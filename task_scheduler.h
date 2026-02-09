#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <QObject>
#include <QStringList>
#include "tasklist_panel.h"

class TaskScheduler : public QObject
{
    Q_OBJECT

public:
    explicit TaskScheduler(QObject *parent = nullptr);
    ~TaskScheduler();

    void setTaskListPanel(TaskListPanel *panel);
    void setAutoPrintEnabled(bool enabled);
    void setCurrentTask(const QString &taskName);
    QString getNextTask();
    void addTask(const QString &task);
    void removeTask(const QString &task);
    void clearTasks();
    int taskCount() const;

signals:
    void taskReady(const QString &taskName);
    void taskQueueEmpty();

public slots:
    void onTaskListUpdated();
    void checkAndScheduleTask();

private:
    TaskListPanel *m_taskListPanel;
    QStringList m_taskQueue;
    bool m_autoPrintEnabled;
    QString m_currentTask;
};

#endif // TASK_SCHEDULER_H