#include "task_table_model.h"

TaskTableModel::TaskTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << tr("序号") << tr("任务名") << tr("状态") << tr("信息") << tr("内容");
}

int TaskTableModel::rowCount(const QModelIndex &) const {
    return m_tasks.count();
}

int TaskTableModel::columnCount(const QModelIndex &) const {
    return 5;  // index, name, statusText, info, content
}

QVariant TaskTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_tasks.size())
        return QVariant();

    const TaskData &task = m_tasks[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return index.row() + 1;
        case 1: return task.name;
        case 2: return task.statusText;
        case 3: return task.info;
        case 4: return task.content;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;  // 所有列都居中对齐
        }

    if (role == Qt::UserRole && index.column() == ColNum::COL_CONTENT) {
        return task.name;  // 用于 signal_task_info
    }

    // 状态图标用 UserRole + 1 返回原始状态枚举
    if (role == Qt::UserRole + 1 && index.column() == 2) {
        return QVariant::fromValue(task.status);
    }

    return QVariant();
}

QVariant TaskTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        // 固定前两列（不允许用户改）
        if (section == ColNum::COL_INDEX) return tr("全选");
        if (section == ColNum::COL_NAME)  return tr("任务名");

        // 后三列允许用户动态设置
        int dynamicIndex = section - ColNum::COL_STA;  // 从 index 0 开始取 m_headers
        if (dynamicIndex >= 0 && dynamicIndex < m_headers.size())
            return m_headers[dynamicIndex];
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}


Qt::ItemFlags TaskTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TaskTableModel::addTask(const TaskData &task, bool insertFront) {
    int pos = insertFront ? 0 : m_tasks.count();

    beginInsertRows(QModelIndex(), pos, pos);
    if (insertFront)
        m_tasks.prepend(task);
    else
        m_tasks.append(task);
    endInsertRows();
}

void TaskTableModel::addTasks(const QList<TaskData> &tasks, bool insertFront)
{
    if (tasks.isEmpty())
        return;

    int pos = insertFront ? 0 : m_tasks.count();

    beginInsertRows(QModelIndex(), pos, pos + tasks.count() - 1);
    if (insertFront)
        m_tasks = tasks + m_tasks;
    else
        m_tasks.append(tasks);
    endInsertRows();
}


void TaskTableModel::removeTasks(const QList<int> &rows) {
    // 空列表检查，避免不必要的排序操作
    if (rows.isEmpty()) {
        return;
    }
    
    QList<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted) {
        if (row >= 0 && row < m_tasks.size()) {
            beginRemoveRows(QModelIndex(), row, row);
            m_tasks.removeAt(row);
            endRemoveRows();
        }
    }
}

TaskData TaskTableModel::getTask(int row) const
{
    return (row >= 0 && row < m_tasks.size()) ? m_tasks[row] : TaskData();
}

TaskData &TaskTableModel::getTaskRef(int row)
{
    // 在发布模式下，Q_ASSERT不会执行，所以添加额外的安全检查
    if (row < 0 || row >= m_tasks.size())
    {
        // 如果索引无效，返回第一个元素或创建一个临时对象
        static TaskData emptyTask;
        return emptyTask;
    }
    return m_tasks[row];
}

void TaskTableModel::updateTaskStatus(int row, PrintSta newStatus)
{
    if (row < 0 || row >= m_tasks.size())
        return;

    TaskData &task = m_tasks[row];
    task.status = newStatus;
    task.statusText = sta_display_str(newStatus);

    QModelIndex topLeft = index(row, ColNum::COL_STA);
    emit dataChanged(topLeft, topLeft, { Qt::DisplayRole, Qt::UserRole + 1 });
}

int TaskTableModel::updateTaskFinished(int row)
{
    if (row < 0 || row >= m_tasks.size())
        return -1;

    TaskData &task = m_tasks[row];
    //task.name = newName;
    task.status = PRINT_STA_WAIT;
    task.statusText = sta_display_str(PRINT_STA_WAIT);

    emit dataChanged(index(row, COL_NAME), index(row, COL_NAME));
    emit dataChanged(index(row, COL_STA), index(row, COL_STA),
                     { Qt::DisplayRole, Qt::UserRole + 1 });

    return row;
}

void TaskTableModel::updateTaskData(int row, const TaskData &updated)
{
    if (row < 0 || row >= m_tasks.size())
        return;

    m_tasks[row] = updated;
    QModelIndex topLeft = index(row, ColNum::COL_STA);
    emit dataChanged(topLeft, topLeft, { Qt::DisplayRole, Qt::UserRole + 1 });
}

void TaskTableModel::clearAllTasks()
{
    if (m_tasks.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_tasks.count() - 1);
    m_tasks.clear();
    endRemoveRows();
}


void TaskTableModel::setHeaderText(const QStringList &headers)
{
    m_headers = headers;
    emit headerDataChanged(Qt::Horizontal, 0, m_headers.size() - 1);
}


QString TaskTableModel::sta_display_str(int sta)
{
    //正数作打印次数用
    if(sta >= 0)
    {
        return QString::number(sta);
    }

    //负数作打印状态用
    QString str_sta;
    switch (sta) {
    case PrintSta::PRINT_STA_WAIT:
        str_sta = QString(tr("等待"));
        break;
    case PrintSta::PRINT_STA_FROZEN:
        str_sta = QString(tr("冻结"));
        break;
    case PrintSta::PRINT_STA_PRINTING:
        str_sta = QString(tr("打印中"));
        break;
    default:
        str_sta = QString(tr("未知"));
    }
    return str_sta;
}



