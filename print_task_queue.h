// ==============================
// print_task_queue.h
// ==============================
#ifndef PRINT_TASK_QUEUE_H
#define PRINT_TASK_QUEUE_H

#include <QQueue>
#include <QSet>
#include <QString>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <atomic>

#include "MarkHandle.h"

/*=====================================================
m_currentProcessingTask ← dequeue() 设置
    ↓
打印开始
    ↓
[正常完成] → taskFinished() → 清除 m_currentProcessingTask
[异常退出] → EXCEPTION → 未清除 m_currentProcessingTask
    ↓
[用户重新开始] → dequeue() 检查任务状态
    ↓
若任务已完成 → 自动清除 → 可继续下一任务


[ UI / 文件加载 ]
        ↓
   enqueue() / enqueueFront()
        ↓
  ┌─────────────┐
  │ m_taskQueue │ ◄──←────┐
  └─────────────┘         │
        ↓                ↑
     dequeue()       recoverIfTaskAlreadyFinished()
        ↓
m_currentProcessingTask
        ↓
 handle_task_xxx()
        ↓
 taskFinished()
        ↓
m_currentProcessingTask ← cleared
        ↓
next dequeue()
 ======================================================*/

class TaskListPanel; // forward declaration

class PrintTaskQueue : public QObject
{
    Q_OBJECT
public:
    static PrintTaskQueue& getInstance()
    {
        static PrintTaskQueue queue;
        return queue;
    }

    void setTaskSourcePanel(TaskListPanel* panel);
    void setMarkSourceHandle(MarkHandle * handle);

    void enqueue(const QString& taskName);
    void enqueueFront(const QString& taskName);

    QString dequeue();
    void taskFinished(const QString& taskName);

    void refreshFromTaskPanel();

    // 查询当前任务状态
    QString currentProcessingTask() ;

    bool isPaused() const { return m_paused; }

    bool isProcessing() ;
    void pause();   // 外部主动挂起当前任务处理
    void resume();  // 恢复任务处理

    // 异常恢复检测：若当前任务已完成，则重置处理状态
    void recoverIfTaskAlreadyFinished();

    void removeTask(const QString& taskName);
    void debugStatus();
    void shutdown();
    QStringList allTasks();

    // 退出控制接口
    void requestExit();
    bool exitRequested() const;

    bool isTaskValid(const QString& taskName) const;
private:
    PrintTaskQueue() = default;
    ~PrintTaskQueue();
    void refreshFromTaskPanel_locked();
    void insertTaskByArrangeOrder(const QString& taskName);
    PrintTaskQueue(const PrintTaskQueue&) = delete;
    PrintTaskQueue& operator=(const PrintTaskQueue&) = delete;
    void dispatchNext();

    QQueue<QString> m_taskQueue;
    QSet<QString> m_activeTasks;
    QString m_currentProcessingTask; // 当前正在执行的任务
    bool m_paused = false;

    QMutex m_mutex;
    QWaitCondition m_condition;

    TaskListPanel* m_sourcePanel = nullptr;

    MarkHandle * m_pMarkHandle = nullptr;

    std::atomic_bool m_exitRequested { false };
};

#endif // PRINT_TASK_QUEUE_H
