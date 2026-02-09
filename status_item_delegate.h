#ifndef STATUS_ITEM_DELEGATE_H
#define STATUS_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include "task_table_model.h"  // 包含 TaskData 和 PrintSta

class StatusItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit StatusItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    QPixmap getIcon(PrintSta status) const;
};

#endif // STATUS_ITEM_DELEGATE_H

