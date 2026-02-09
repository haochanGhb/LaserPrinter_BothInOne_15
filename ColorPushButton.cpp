#include "ColorPushButton.h"



ColorPushButton::ColorPushButton(QWidget *parent) : QPushButton(parent)
{
    m_buttonType = ButtonType::TissueCassette;
    m_backgroundColor = Qt::white;
    m_borderColor = QColor(116,116,116);
}

void ColorPushButton::setButtonType(ButtonType type)
{
    m_buttonType = type;
}

void ColorPushButton::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

// 获取背景颜色方法
QColor ColorPushButton::getBackgroundColor() const
{
    return m_backgroundColor;
}

void ColorPushButton::paintEvent(QPaintEvent *event)
{
    // 获取当前状态颜色
    if (isDown())
    {// 按下
        m_borderColor = QColor(50, 50, 50);
    }
    else if (underMouse())
    {// 悬停
        m_borderColor = QColor(50, 50, 50);
    }
    else
    {// 默认
        m_borderColor = QColor(116,116,116);
    }

    if (m_buttonType == TissueCassette)
    {
        QPainter painter(this);
        //painter.setRenderHint(QPainter::Antialiasing);

        // 按钮背景
        painter.fillRect(rect(), m_backgroundColor);

        // 边框
        painter.setPen(QPen(m_borderColor, 2));
        QRect border_rect = rect().adjusted(0, 0, 0, 0);
        painter.drawRect(border_rect);

        // 中间间隔线
        painter.setPen(QPen(m_borderColor, 1));
        painter.drawLine(0, height()*0.8, width(), height()*0.8);

        // 上半部分网格效果
        painter.setPen(QPen(m_borderColor, 1));

        painter.drawLine(width()*0.2, height()*0.2, width()*0.4, height()*0.2);
        painter.drawLine(width()*0.2, height()*0.4, width()*0.4, height()*0.4);
        painter.drawLine(width()*0.2, height()*0.6, width()*0.4, height()*0.6);

        painter.drawLine(width()*0.6, height()*0.2, width()*0.8, height()*0.2);
        painter.drawLine(width()*0.6, height()*0.4, width()*0.8, height()*0.4);
        painter.drawLine(width()*0.6, height()*0.6, width()*0.8, height()*0.6);
    }

    if (m_buttonType == Slider)
    {
        QPainter painter(this);
        //painter.setRenderHint(QPainter::Antialiasing);

        // 按钮背景
        painter.fillRect(rect(), Qt::white);

        painter.setPen(QPen(m_borderColor, 1));

        // 上半部分
        QRect topRect(width()*0.2, 0, width()*0.6, height()-(width()*0.4));
        QLinearGradient gradient(topRect.topLeft(), topRect.bottomLeft());
        gradient.setColorAt(0.0, Qt::white);            // 顶部颜色
        gradient.setColorAt(1.0, QColor(223,245,251));  // 底部颜色
        painter.setBrush(gradient);                     // 使用渐变画刷
        painter.drawRect(topRect);

        // 下半部分
        //painter.setPen(Qt::NoPen);
        QRect bottomRect(width()*0.2, height()-width()*0.4, width()*0.6, width()*0.4);
        painter.setBrush(Qt::NoBrush);
        painter.fillRect(bottomRect, m_backgroundColor);
        painter.drawRoundedRect(bottomRect, 1, 1);


//        QRect bottomRect(0, 0, width()-1, height()-1);
//        painter.setBrush(Qt::NoBrush);
//        painter.fillRect(bottomRect, m_backgroundColor);
//        painter.drawRoundedRect(bottomRect, 1, 1);

//        painter.setPen(Qt::NoPen);
//        QRect topRect(1, 0, width()-2, height()*0.4);
//        QLinearGradient gradient(topRect.topLeft(), topRect.bottomLeft());
//        gradient.setColorAt(0.0, Qt::white);            // 顶部颜色
//        gradient.setColorAt(1.0, QColor(223,245,251));  // 底部颜色
//        painter.setBrush(gradient);                     // 使用渐变画刷
//        painter.drawRect(topRect);

    }
}

void  ColorPushButton::resizeEvent(QResizeEvent* e)
{
    if (width() > height() * 0.8)
    {
        int newWidth = height() * 0.8;
        setFixedWidth(newWidth);
    }
    else
    {
        int newHeight = width() / 0.8;
        setFixedHeight(newHeight);
    }


}
