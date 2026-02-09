#include "task_scheduler.h"

TaskScheduler::TaskScheduler(QObject *parent) : QObject(parent), m_taskListPanel(nullptr), m_autoPrintEnabled(false)
{
}

TaskScheduler::~TaskScheduler()
{
}

void TaskScheduler::setTaskListPanel(TaskListPanel *panel)
{
    m_taskListPanel = panel;
    if (m_taskListPanel) {
        // 这里可以添加与任务列表面板的信号连接
    }
}

void TaskScheduler::setAutoPrintEnabled(bool enabled)
{
    m_autoPrintEnabled = enabled;
}

void TaskScheduler::setCurrentTask(const QString &taskName)
{
    m_currentTask = taskName;
}

QString TaskScheduler::getNextTask()
{
    if (m_taskQueue.isEmpty()) {
        if (m_taskListPanel) {
            // 从task_model中获取第一个等待状态的任务
            QString firstTask = m_taskListPanel->getFirstWaitingTaskNameFromModel();
            if (!firstTask.isEmpty()) {
                m_taskQueue.append(firstTask);
            }
        }
    }

    //这里的m_taskQueue要处理任务在短时间内被重复取后重复执行的问题
    if (!m_taskQueue.isEmpty()) {
        QString nextTask = m_taskQueue.takeFirst();
        emit taskReady(nextTask);
        return nextTask;
    }

    emit taskQueueEmpty();
    return QString();
}

void TaskScheduler::addTask(const QString &task)
{
    m_taskQueue.append(task);
}

void TaskScheduler::removeTask(const QString &task)
{
    m_taskQueue.removeAll(task);
}

void TaskScheduler::clearTasks()
{
    m_taskQueue.clear();
    emit taskQueueEmpty();
}

int TaskScheduler::taskCount() const
{
    return m_taskQueue.size();
}

void TaskScheduler::onTaskListUpdated()
{
    // 当任务列表更新时的处理
    if (m_taskListPanel) {
        // 可以在这里更新任务队列
    }
}

void TaskScheduler::checkAndScheduleTask()
{
    // 检查是否启用了自动打印
    if (!m_autoPrintEnabled) {
        return;
    }

    // 检查task_model是否有等待任务
    if (m_taskListPanel)
    {
        if(!m_currentTask.isEmpty())
        {
            //有任务在执行
            return;
        }

        QString firstTask = m_taskListPanel->getFirstWaitingTaskNameFromModel();

        if (!firstTask.isEmpty())
        {
            qDebug() << "checkAndScheduleTask.firstTask=======>>>>>>" << firstTask;
            // 检查这个任务是否是当前正在处理的任务
            // 如果是当前任务，则不重复发送信号
            if (firstTask != m_currentTask)
            {
                emit taskReady(firstTask);
            }
            else
            {
                qDebug() << "checkAndScheduleTask: 任务" << firstTask << "是当前正在处理的任务，跳过";
            }
        }
    }
}
