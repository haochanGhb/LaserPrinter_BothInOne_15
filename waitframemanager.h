#ifndef WAITFRAMEMANAGER_H
#define WAITFRAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>

class HintOverlay;

class WaitFrameManager : public QObject
{
    Q_OBJECT
public:
    static WaitFrameManager& instance();

    void showWaitFrame(const QString& tip);
    void stopWaitFrame();
    void setUseSimpleOverlay(bool enabled);

signals:
    void waitFrameStarted();       // 动画已启动
    void waitFrameClosed();        // 动画关闭
    void waitFrameError(const QString& msg); // 可选：提示信息

private:
    explicit WaitFrameManager(QObject* parent = nullptr);

    WaitFrameManager(const WaitFrameManager&) = delete;
    WaitFrameManager& operator=(const WaitFrameManager&) = delete;
    HintOverlay * m_hintOverlay = nullptr;

    bool m_useSimpleOverlay;
};

#endif // WAITFRAMEMANAGER_H

