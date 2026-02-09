#ifndef TISSUECASSETTEPUSHBUTTON_H
#define TISSUECASSETTEPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>

typedef enum
{
    TissueCassette,     // 组织盒
    Slider,             // 载玻片
} ButtonType;

class ColorPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorPushButton(QWidget* parent = nullptr);

    void setButtonType(ButtonType type);
    void setBackgroundColor(const QColor &color);
    QColor getBackgroundColor() const; // 获取背景颜色

private:
    ButtonType m_buttonType;

    QColor m_backgroundColor;
    QColor m_borderColor;

protected:
    // 重写绘图事件
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;

};

#endif // TISSUECASSETTEPUSHBUTTON_H
