#ifndef TOUCH_KEYBOARD_CONTROLLER_H
#define TOUCH_KEYBOARD_CONTROLLER_H

#pragma once
#include <QObject>
#include <Windows.h>
#include <QDebug>
#include <QWidget>
#include <QTimer>
#include <QDateTime>

#define TOUCH_KEYBORAD_DISABLED 0
static const int STABLE_DELAY_MS = 300;   // 焦点稳定时间

class TouchKeyboardController {
public:
    // 全局标记触摸键盘是否启用
    static inline bool s_enabled = true;

private:
    QTimer  m_timer ;               // singleShot 定时器
    bool    m_tabTipLaunched ; // 是否已启动（未使用，可选增加判断避免重复调用）
    QWidget * m_lineEdit;


public:

    // 在类中添加静态初始化函数声明
    void initialize()
    {
        // 执行必要的初始化逻辑
//        m_timer.setSingleShot(true);
//        QObject::connect(&m_timer, &QTimer::timeout, []() {
//            if (!m_lineEdit) return;

//            if (s_enabled) {
//                allowTouchKeyboard(m_lineEdit);
//                show();
//            }
//            else {
//                suppressTouchKeyboard(m_lineEdit);
//                hide();
//            }
//        });
//        m_tabTipLaunched = false;
//        m_lineEdit = nullptr;
    }

    static void setEnabled(bool enabled) {
        s_enabled = enabled;
    }

    static bool isEnabled() {
        return s_enabled;
    }

    // 外部调用 → FocusIn 时处理
    static void handleFocusIn(QWidget* w) {

        if(TOUCH_KEYBORAD_DISABLED) return;


        if (!w) return;

        if (s_enabled) {
            allowAutoInvoke(w);
            show();
        }
        else {
            suppressAutoInvoke(w);
            hide();
        }

    }

    // 外部调用 → FocusOut 时处理
    static void handleFocusOut(QWidget* w) {
        if(TOUCH_KEYBORAD_DISABLED) return;
        if (!w) return;
        
        // 失去焦点时，禁用自动调用以脱钩
        suppressAutoInvoke(w);
    }

public: // 显示与隐藏
    static void show() {

               if(TOUCH_KEYBORAD_DISABLED) return;

        if (!GetForegroundWindow())
            return;

        ShellExecuteW(NULL, L"open", L"TabTip.exe", NULL, NULL, SW_SHOWNORMAL);
    }

    static void hide() {
               if(TOUCH_KEYBORAD_DISABLED) return;

        HWND hwnd = FindWindow(L"IPTIP_Main_Window", nullptr);
        if (hwnd) {
            PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            qDebug() << "Touch keyboard closed";
        }
    }

public: // 控制自动弹出策略
    static void suppressAutoInvoke(QWidget* w)
    {
               if(TOUCH_KEYBORAD_DISABLED) return;
        HWND hwnd = (HWND)w->winId();
        ::SetProp(hwnd, L"DisableTouchKeyboardAutoInvoke", (HANDLE)TRUE);
        //w->setAttribute(Qt::WA_InputMethodEnabled, false);
    }

    static void suppressTouchKeyboard(QWidget* w)
    {
               if(TOUCH_KEYBORAD_DISABLED) return;
        if (!w) return;
        
        HWND hwnd = (HWND)w->winId();
        // 禁用自动弹出策略
        ::SetProp(hwnd, L"DisableTouchKeyboardAutoInvoke", (HANDLE)TRUE);
    }

    static void allowAutoInvoke(QWidget* w)
    {
               if(TOUCH_KEYBORAD_DISABLED) return;
        HWND hwnd = (HWND)w->winId();
        ::RemoveProp(hwnd, L"DisableTouchKeyboardAutoInvoke");

        //w->setAttribute(Qt::WA_InputMethodEnabled, true);
    }

    static void allowTouchKeyboard(QWidget* w)
    {
               if(TOUCH_KEYBORAD_DISABLED) return;
        if (!w) return;
        HWND hwnd = (HWND)w->winId();
        ::RemoveProp(hwnd, L"DisableTouchKeyboardAutoInvoke");
    }

};


#endif // TOUCH_KEYBOARD_CONTROLLER_H
