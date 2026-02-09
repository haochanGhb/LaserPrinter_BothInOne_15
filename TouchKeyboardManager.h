#ifndef TOUCHKEYBOARDMANAGER_H
#define TOUCHKEYBOARDMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

#define TOUCH_KEYBORAD_DISABLED 0

class TouchKeyboardManager : public QObject
{

    Q_OBJECT
public:
    static TouchKeyboardManager& instance();



    static void setEnabled(bool enabled) {
        s_enabled = enabled;
    }

    static bool isEnabled() {
        return s_enabled;
    }

    void requestShow(QWidget * w);
    // 外部调用 → FocusIn 时处理
    void handleFocusIn(QWidget* w);

    // 外部调用 → FocusOut 时处理
    void handleFocusOut(QWidget* w);

    // 显示与隐藏
    static void show();
    static void hide();

    // 控制自动弹出策略
    static void suppressAutoInvoke(QWidget* w);
    static void allowAutoInvoke(QWidget* w);
    static void suppressTouchKeyboard(QWidget* w);
    static void allowTouchKeyboard(QWidget* w);

    // 全局标记触摸键盘是否启用
    static inline bool s_enabled = true;

private slots:
    void onTimeout();

private:
    explicit TouchKeyboardManager(QObject* parent = nullptr);
    void showTabTip();
    void hideTabTip();

private:
    QTimer  m_timer;               // singleShot 定时器
    bool    m_tabTipLaunched = false; // 是否已启动（未使用，可选增加判断避免重复调用）
    QWidget * m_lineEdit = nullptr;
};

#endif // TOUCHKEYBOARDMANAGER_H
