#include "history_table_model.h"

HistoryTableModel::HistoryTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers  << tr("全选") << tr("任务名") << tr("状态") << tr("信息") << tr("内容") << tr("时间") ;
}

int HistoryTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_history.size();
}

int HistoryTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_headers.size();
}

QVariant HistoryTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    int r = index.row();
    if (r < 0 || r >= m_history.size()) return QVariant();
    const HistoryData &it = m_history.at(r);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case COL_INDEX:    return r + 1;           // 显示序号
        case COL_NAME:     return it.name;         // 隐藏列
        case COL_STA:      return it.sta;          // 隐藏列
        case COL_INFO:     return it.info;
        case COL_CONTENT:  return it.content;
        case COL_DATETIME: return it.datetime;
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    // 保留原有行为：在 COL_INDEX 对应的 QModelIndex 上用 UserRole 返回 GUID
    if (role == Qt::UserRole && index.column() == COL_INDEX)
        return it.guid;

    // 如需返回原始文本供委托或导出使用，可用 UserRole+1
    if (role == Qt::UserRole + 1)
        return it.fullText;

    return QVariant();
}


QVariant HistoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section >= 0 && section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}

Qt::ItemFlags HistoryTableModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void HistoryTableModel::setHistoryData(const QList<HistoryData> &data) {
    beginResetModel();
    m_history = data;
    endResetModel();
}

void HistoryTableModel::removeTableRows(const QList<int> &rows) {
    QList<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted) {
        if (row >= 0 && row < m_history.size()) {
            beginRemoveRows(QModelIndex(), row, row);
            m_history.removeAt(row);
            endRemoveRows();
        }
    }
}

HistoryData HistoryTableModel::getHistory(int row) const {
    if (row < 0 || row >= m_history.size())
        return HistoryData();
    return m_history[row];
}

void HistoryTableModel::setHeaders(const QStringList &headers) {
    m_headers = headers;
    emit headerDataChanged(Qt::Horizontal, 0, m_headers.size() - 1);
}

void HistoryTableModel::clearAllTasks()
{
    if (m_history.isEmpty())
        return;

    beginResetModel();
    m_history.clear();
    endResetModel();
}

void HistoryTableModel::appendHistoryRow(const HistoryData &data)
{
    int row = m_history.size();
    beginInsertRows(QModelIndex(), row, row);
    m_history.append(data);
    endInsertRows();
}



