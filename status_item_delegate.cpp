#include "status_item_delegate.h"

StatusItemDelegate::StatusItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

QPixmap StatusItemDelegate::getIcon(PrintSta status) const {
    static QPixmap waitingIcon(":/image/waiting.png");
    static QPixmap printingIcon(":/image/printing.png");

    if (status == PRINT_STA_WAIT) {
        return waitingIcon;
    } else if (status == PRINT_STA_PRINTING) {
        return printingIcon;
    }
    return QPixmap();  // 默认空
}

void StatusItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {

    if (!index.isValid())
        return;

    painter->save();

    // 绘制背景
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, QColor("#d9f0f8"));
    else
        painter->fillRect(option.rect, Qt::white);

    // 获取状态和文本
    PrintSta status = static_cast<PrintSta>(index.data(Qt::UserRole + 1).toInt());
    QString text = index.data(Qt::DisplayRole).toString();
    QPixmap icon = getIcon(status);

    // 判断是否有图标
    bool hasIcon = !icon.isNull();
    int iconSize = 24;
    int iconWidth = hasIcon ? iconSize : 0;
    int spacing = hasIcon ? 6 : 0;

    // 准备文字
    QFont font("微软雅黑", 12);
    painter->setFont(font);
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(text);

    // 总宽度 = 图标 + 间距 + 文本
    int totalWidth = iconWidth + spacing + textWidth;
    int startX = option.rect.left() + (option.rect.width() - totalWidth) / 2;
    int centerY = option.rect.center().y();

    // 计算绘图区域
    QRect iconRect(startX, centerY - iconSize / 2, iconWidth, iconSize);
    QRect textRect(iconRect.right() + spacing, option.rect.top(),
                   option.rect.right() - iconRect.right() - spacing, option.rect.height());

    // 绘图标
    if (hasIcon)
        painter->drawPixmap(iconRect, icon);

    // 绘文本
    painter->setPen(QColor("#2a2d32"));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

    painter->restore();

}

QSize StatusItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
    return QSize(option.rect.width(), 63);  // 与原 IconTextCellWidget 一致高度
}
