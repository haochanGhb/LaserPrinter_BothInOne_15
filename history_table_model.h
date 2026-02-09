#ifndef HISTORYTABLEMODEL_H
#define HISTORYTABLEMODEL_H


#pragma once

#include <QAbstractTableModel>
#include <QStringList>
#include <QBrush>
#include <QColor>
#include "public_fun.h"

// 历史记录数据结构
struct HistoryData {
    QString guid;       // 原查询 field 0 的 GUID —— 用于 UserRole（原先存在 COL_INDEX 的 UserRole）
    QString name;       // COL_NAME （隐藏列，旧结构保留）
    QString sta;        // COL_STA  （隐藏列，旧结构保留）
    QString info;       // COL_INFO
    QString content;    // COL_CONTENT
    QString datetime;   // COL_DATETIME（格式化字符串）
    QString fullText;   // 可选：保存原始 task_text
};


class HistoryTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit HistoryTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setHistoryData(const QList<HistoryData> &data);
    void removeTableRows(const QList<int> &rows) ;
    HistoryData getHistory(int row) const;

    void setHeaders(const QStringList &headers);
    void clearAllTasks();

    void appendHistoryRow(const HistoryData &data);

private:
    QList<HistoryData> m_history;
    QStringList m_headers;
};

#endif // HISTORYTABLEMODEL_H
