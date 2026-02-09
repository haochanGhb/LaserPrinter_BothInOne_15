#ifndef TASKTABLEMODEL_H
#define TASKTABLEMODEL_H


#include <QAbstractTableModel>
#include "public_fun.h"

struct TaskData {
    QString name;
    QString statusText;
    PrintSta status;
    QString info;
    QString content;
    QString fullText;
};

class TaskTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TaskTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addTask(const TaskData &task, bool insertFront = false);
    void addTasks(const QList<TaskData> &tasks, bool insertFront = false);
    void removeTasks(const QList<int> &rows);
    void updateTaskStatus(int row, PrintSta newStatus);
    //int updateTaskFinished(int row, const QString &newName);
    int updateTaskFinished(int row);
    void updateTaskData(int row, const TaskData &updated);
    TaskData getTask(int row) const;
    TaskData & getTaskRef(int row);

    void setHeaderText(const QStringList &headers);
    void clearAllTasks();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QString sta_display_str(int sta);
private:
    QList<TaskData> m_tasks;
    QStringList m_headers;
};

#endif // TASK_TABLE_MODEL_H
