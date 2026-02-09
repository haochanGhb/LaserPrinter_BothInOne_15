// ==============================
// print_task_queue.cpp
// ==============================
#include "print_task_queue.h"
#include "tasklist_panel.h"

void PrintTaskQueue::setTaskSourcePanel(TaskListPanel* panel)
{
    QMutexLocker locker(&m_mutex);
    m_sourcePanel = panel;
}

void PrintTaskQueue::setMarkSourceHandle(MarkHandle * handle)
{
    QMutexLocker locker(&m_mutex);
    m_pMarkHandle = handle;
}

void PrintTaskQueue::enqueue(const QString& taskName)
{
    if (taskName.isEmpty()) return;
    QMutexLocker locker(&m_mutex);
    if (!m_activeTasks.contains(taskName))
    {
        m_taskQueue.enqueue(taskName);
        m_activeTasks.insert(taskName);
        m_condition.wakeOne();
    }
}

void PrintTaskQueue::enqueueFront(const QString& taskName)
{
    if (taskName.isEmpty()) return;
    QMutexLocker locker(&m_mutex);
    if (!m_activeTasks.contains(taskName))
    {
        m_taskQueue.prepend(taskName);
        m_activeTasks.insert(taskName);
        m_condition.wakeOne();
    }
}

QString PrintTaskQueue::dequeue()
{
       QMutexLocker locker(&m_mutex);

       recoverIfTaskAlreadyFinished(); // 检查是否需要恢复

       while (m_taskQueue.isEmpty() || !m_currentProcessingTask.isEmpty()) {
           m_condition.wait(&m_mutex, 100); //超时等待1000==>>100

           recoverIfTaskAlreadyFinished(); // 防止唤醒后卡住

           if (m_exitRequested.load())
           {
               m_exitRequested.store(false);
               qDebug() << "[PrintTaskQueue] dequeue 检测到退出标志，返回空任务";
               return QString();
           }

       }

       m_currentProcessingTask = m_taskQueue.dequeue();
       return m_currentProcessingTask;
}

void PrintTaskQueue::taskFinished(const QString& taskName)
{
    QMutexLocker locker(&m_mutex);
    if (taskName == m_currentProcessingTask)
    {
        m_activeTasks.remove(taskName);
        m_currentProcessingTask.clear();
        m_condition.wakeOne(); // 唤醒等待中的 dequeue
    }
}

void PrintTaskQueue::refreshFromTaskPanel()
{
    QMutexLocker locker(&m_mutex);
    refreshFromTaskPanel_locked();
}

void PrintTaskQueue::refreshFromTaskPanel_locked()
{
    if (!m_sourcePanel) return;

    QStringList orderedTasks = m_sourcePanel->getAllWaitingTaskNames();
    for (const QString& taskName : orderedTasks)
    {
        if (!m_activeTasks.contains(taskName))
        {
            m_taskQueue.enqueue(taskName);
            m_activeTasks.insert(taskName);
        }
    }
}

QString PrintTaskQueue::currentProcessingTask()
{
    QMutexLocker locker(&m_mutex);
    return m_currentProcessingTask;
}

QStringList PrintTaskQueue::allTasks()
{
    QMutexLocker locker(&m_mutex);
    QStringList tasks;
    foreach (const QString& task, m_taskQueue) {
        tasks.append(task);
    }
    return tasks;
}

bool PrintTaskQueue::isProcessing()
{
    QMutexLocker locker(&m_mutex);
    return !m_currentProcessingTask.isEmpty();
}

bool PrintTaskQueue::isTaskValid(const QString& taskName) const
{
    return m_activeTasks.contains(taskName) || m_taskQueue.contains(taskName);
}

void PrintTaskQueue::pause()
{
    QMutexLocker locker(&m_mutex);
    if (!m_currentProcessingTask.isEmpty()) {
        qDebug() << "[PrintTaskQueue] 正在处理任务，暂停处理流程:" << m_currentProcessingTask;

//        if (m_pMarkHandle)
//        {
//            m_pMarkHandle->stopMark();
//        }

      m_paused = true;
      requestExit();
    }
}

void PrintTaskQueue::resume()
{
    if (m_paused) {
        //m_paused = false;
        //dispatchNext();  // 可立即重新尝试调度
        recoverIfTaskAlreadyFinished();

        QTimer::singleShot(200, this, [this]() {
             qDebug() << "[PrintTaskQueue] 任务调度已恢复";
             m_paused = false;
         });
    }
}


void PrintTaskQueue::dispatchNext()
{
    if (m_paused || m_currentProcessingTask != "") {
        return; // 暂停状态或已有任务在执行
    }

    if (m_taskQueue.isEmpty()) return;

    m_currentProcessingTask = m_taskQueue.takeFirst();
    m_activeTasks.insert(m_currentProcessingTask);

    qDebug() << "[PrintTaskQueue] 开始执行任务:" << m_currentProcessingTask;

    // 此处调用串口或其他业务模块执行任务逻辑...
}


void PrintTaskQueue::recoverIfTaskAlreadyFinished()
{
    if (m_currentProcessingTask.isEmpty() || !m_sourcePanel)
        return;

    try
    {
        // Check if the task is still in the queue and its status
        auto status = m_sourcePanel->getTaskRecoverStatus(m_currentProcessingTask);
//        auto status = m_sourcePanel->getTaskRecoverStatusFromModel(m_currentProcessingTask);
        switch (status)
        {
        case TaskRecoverStatus::WaitingAndIsFirst:
            qDebug() << "[PrintTaskQueue] 异常恢复：任务仍处于队首 WAIT 状态，重新排队:" << m_currentProcessingTask;
            m_taskQueue.prepend(m_currentProcessingTask);
            break;

        case TaskRecoverStatus::WaitingNotFirst:
            qDebug() << "[PrintTaskQueue] 异常恢复：任务仍在等待，但位置靠后，按顺序恢复:" << m_currentProcessingTask;
            insertTaskByArrangeOrder(m_currentProcessingTask);
            break;

        case TaskRecoverStatus::NotExistOrFinished:
        default:
            qDebug() << "[PrintTaskQueue] 异常恢复：任务已完成或不存在，移除:" << m_currentProcessingTask;
            m_activeTasks.remove(m_currentProcessingTask);
            break;
        }

        m_currentProcessingTask.clear();
    }
    catch (const std::exception& e)
    {
        qDebug() << "[PrintTaskQueue] recoverIfTaskAlreadyFinished() 异常:" << e.what();
        m_currentProcessingTask.clear();
    }
}

void PrintTaskQueue::removeTask(const QString& taskName)
{
    if (taskName.isEmpty()) return;

    QMutexLocker locker(&m_mutex);

    // 如果删除的是当前正在执行的任务
    if (taskName == m_currentProcessingTask) {
        qDebug() << "[PrintTaskQueue] 删除当前执行任务:" << taskName;
        m_activeTasks.remove(taskName);
        m_currentProcessingTask.clear();
        m_condition.wakeOne();
        return;
    }

    // 否则正常从队列中删除
    if (m_taskQueue.removeAll(taskName) > 0) {
        qDebug() << "[PrintTaskQueue] 删除等待任务:" << taskName;
        m_activeTasks.remove(taskName);
    }
}

void PrintTaskQueue::insertTaskByArrangeOrder(const QString& taskName)
{
    if (!m_sourcePanel) return;

    QStringList arrangeList = m_sourcePanel->getTaskArrangeOrderFromModel();
//    QStringList arrangeList = m_sourcePanel->getTaskArrangeOrder();
    if (arrangeList.isEmpty()) {
        m_taskQueue.append(taskName); // fallback
        return;
    }

    int taskIndex = arrangeList.indexOf(taskName);
    if (taskIndex == -1) {
        m_taskQueue.append(taskName); // fallback
        return;
    }

    // 在已有队列中查找下一个排在其后面的任务，插入到它前面
    for (int i = 0; i < m_taskQueue.size(); ++i) {
        const QString& t = m_taskQueue[i];
        int indexInArrange = arrangeList.indexOf(t);
        if (indexInArrange != -1 && indexInArrange > taskIndex) {
            m_taskQueue.insert(i, taskName);
            return;
        }
    }

    // 若没找到更靠后的任务，说明该任务在队列中最靠后，直接append
    m_taskQueue.append(taskName);
}

void PrintTaskQueue::debugStatus()
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "========== PrintTaskQueue 状态 ==========";
    qDebug() << "当前任务:" << m_currentProcessingTask;
    qDebug() << "等待队列:" << m_taskQueue;
    qDebug() << "活跃任务集合:" << m_activeTasks;
    qDebug() << "=========================================";
}

void PrintTaskQueue::shutdown()
{
    QMutexLocker locker(&m_mutex);
    m_taskQueue.clear();
    m_activeTasks.clear();
    m_currentProcessingTask.clear();
    m_sourcePanel = nullptr;
    m_condition.wakeAll(); // 唤醒任何阻塞线程
    qDebug() << "[PrintTaskQueue] shutdown complete.";
}

void PrintTaskQueue::requestExit()
{
    m_exitRequested.store(true);
    m_condition.wakeAll();  // 唤醒正在 wait() 的线程
}

bool PrintTaskQueue::exitRequested() const
{
    return m_exitRequested.load();
}

PrintTaskQueue::~PrintTaskQueue()
{
    requestExit();
    shutdown();
    qDebug() << "[PrintTaskQueue] 析构执行完成";
}
