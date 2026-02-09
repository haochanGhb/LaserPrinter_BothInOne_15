#include "waitframemanager.h"

#include <QMetaObject>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QDebug>

class HintOverlay : public QWidget
{
public:
    explicit HintOverlay(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAttribute(Qt::WA_ShowWithoutActivating);

        QLabel *label = new QLabel(this);
        label->setStyleSheet("background-color: rgba(255, 255, 255, 230);"
                             "border: 1px solid gray; padding: 10px; "
                             "font: 14pt 'Microsoft YaHei';");
        label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(label);
        layout->setMargin(20);

        setFixedSize(300, 100);
        move(QApplication::primaryScreen()->availableGeometry().center() - rect().center());

        label->setText("...");
        m_label = label;
    }

    void showMessage(const QString &message)
    {
        m_label->setText(message);
        show();
        raise();
    }

    void hideOverlay() {
        hide();
    }

private:
    QLabel *m_label;
};

WaitFrameManager::WaitFrameManager(QObject *parent)
    : QObject(parent)
{
    m_hintOverlay = new HintOverlay();
    m_hintOverlay->hide();

    m_useSimpleOverlay = false; // 默认使用完整等待窗体

}

WaitFrameManager& WaitFrameManager::instance()
{
    static WaitFrameManager s_instance;
    return s_instance;
}



void WaitFrameManager::setUseSimpleOverlay(bool enabled)
{
    qDebug() << "[WaitFrameManager] 使用轻量提示窗: " << enabled;
    m_useSimpleOverlay = enabled;
}

void WaitFrameManager::showWaitFrame(const QString& tip)
{
    if (m_useSimpleOverlay) {
        QTimer::singleShot(0, this, [=]() {
            m_hintOverlay->showMessage(tip);

            QEventLoop eventloop;
            QTimer::singleShot(200, &eventloop, SLOT(quit()));
            eventloop.exec();

            emit waitFrameStarted();
        });
    } else {

    }
}

void WaitFrameManager::stopWaitFrame()
{
    if (m_useSimpleOverlay) {
        QTimer::singleShot(0, this, [this]() {
            m_hintOverlay->hideOverlay();
            emit waitFrameClosed();
        });
    } else {

    }

}
