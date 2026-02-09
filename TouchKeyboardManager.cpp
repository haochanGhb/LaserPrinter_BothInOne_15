#include "TouchKeyboardManager.h"
#include <QApplication>
#include <QWidget>
#include <QDebug>

#include <windows.h>
#include <Shellapi.h>

static const int STABLE_DELAY_MS = 300;   // 焦点稳定时间

TouchKeyboardManager& TouchKeyboardManager::instance()
{
    static TouchKeyboardManager inst;
    return inst;
}

TouchKeyboardManager::TouchKeyboardManager(QObject* parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout,
            this, &TouchKeyboardManager::onTimeout);
}

void TouchKeyboardManager::requestShow(QWidget * w)
{
    m_lineEdit = w;
    m_timer.start(STABLE_DELAY_MS);

}

void TouchKeyboardManager::onTimeout()
{
    if (!m_lineEdit) return;

    if (s_enabled) {
        allowTouchKeyboard(m_lineEdit);
        showTabTip();
    }
    else {
        suppressTouchKeyboard(m_lineEdit);
        hideTabTip();
    }
}

void TouchKeyboardManager::showTabTip()
{
    if (!GetForegroundWindow())
        return;

    ShellExecuteW(NULL, L"open", L"TabTip.exe", NULL, NULL, SW_SHOWNORMAL);
    qDebug() << "ShellExecuteW LabTip";
}

void TouchKeyboardManager::hideTabTip()
{
    HWND hwnd = FindWindow(L"IPTIP_Main_Window", nullptr);
    if (hwnd) {
        PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        qDebug() << "Touch keyboard closed";
    }
}

// 外部调用 → FocusIn 时处理
void TouchKeyboardManager::handleFocusIn(QWidget* w)
{
//    if (!w) return;

//    if (s_enabled) {
//        allowAutoInvoke(w);
//        show();
//    }
//    else {
//        suppressAutoInvoke(w);
//        hide();
//    }
    requestShow(w);
}

// 外部调用 → FocusOut 时处理
void TouchKeyboardManager::handleFocusOut(QWidget* w)
{
    if (!w) return;
    
    // 失去焦点时，禁用自动调用以脱钩
    suppressAutoInvoke(w);
}

// 显示与隐藏
void TouchKeyboardManager::show()
{
    if (!GetForegroundWindow())
        return;

    ShellExecuteW(NULL, L"open", L"TabTip.exe", NULL, NULL, SW_SHOWNORMAL);
}

void TouchKeyboardManager::hide()
{
    HWND hwnd = FindWindow(L"IPTIP_Main_Window", nullptr);
    if (hwnd) {
        PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        qDebug() << "Touch keyboard closed";
    }
}

// 控制自动弹出策略
void TouchKeyboardManager::suppressAutoInvoke(QWidget* w)
{
    if (!w) return;
    HWND hwnd = (HWND)w->winId();
    ::SetProp(hwnd, L"DisableTouchKeyboardAutoInvoke", (HANDLE)TRUE);
}

void TouchKeyboardManager::allowAutoInvoke(QWidget* w)
{
    if (!w) return;
    HWND hwnd = (HWND)w->winId();
    ::RemoveProp(hwnd, L"DisableTouchKeyboardAutoInvoke");
}

void TouchKeyboardManager::allowTouchKeyboard(QWidget* w)
{
    if (!w) return;
    HWND hwnd = (HWND)w->winId();
    ::RemoveProp(hwnd, L"DisableTouchKeyboardAutoInvoke");
}

void TouchKeyboardManager::suppressTouchKeyboard(QWidget* w)
{
    if (!w) return;

    HWND hwnd = (HWND)w->winId();
    // 禁用自动弹出策略
    ::SetProp(hwnd, L"DisableTouchKeyboardAutoInvoke", (HANDLE)TRUE);
}




