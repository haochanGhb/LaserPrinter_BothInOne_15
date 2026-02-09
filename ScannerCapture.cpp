#include "ScannerCapture.h"
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

ScannerCapture *ScannerCapture::instance()
{
    static ScannerCapture inst;
    return &inst;
}

ScannerCapture::ScannerCapture(QObject *parent)
    : QObject(parent)
{
#ifdef NO_RETURN_KEY_MODE
    flushTimer.setSingleShot(true);

    connect(&flushTimer, &QTimer::timeout, this, [this]()
    {
        if (buffer.length() >= MIN_LENGTH)
        {
            emit scanFinished(buffer);
        }
        buffer.clear();   // 超时丢弃
    });
#endif
}

void ScannerCapture::start()
{
    if (enabled)
        return;

    enabled = true;
    intervalTimer.start();
    qApp->installEventFilter(this);
}

void ScannerCapture::stop()
{
    if (!enabled)
        return;

    enabled = false;
    qApp->removeEventFilter(this);
    buffer.clear();
}

bool ScannerCapture::eventFilter(QObject *obj, QEvent *event)
{
    if (!enabled || event->type() != QEvent::KeyPress)
        return false;

    // 只接收一次顶层对象的事件，避免重复
    if ("QWidgetWindow" != QString(obj->metaObject()->className()))
        return false;

    auto *e = static_cast<QKeyEvent *>(event);

    if (e->isAutoRepeat())
        return false;

    // 忽略功能键
    if (e->text().isEmpty())
        return false;

//    qDebug() << "Key =" << e->key() << "text =" << e->text() << "len = " << e->text().length();

    qint64 delta = intervalTimer.elapsed();
    intervalTimer.restart();

    // 输入太慢，新一轮
    if (delta > MAX_INTERVAL_MS)
        buffer.clear();

    // 回车完成扫码
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        emit keyEnterCaptured();

        if (buffer.length() >= MIN_LENGTH)
            emit scanFinished(buffer);

        buffer.clear();
#ifdef NO_RETURN_KEY_MODE
        flushTimer.stop();
#endif
        return false;   // 不影响控件
    }

    // 普通字符
    buffer += e->text();

#ifdef NO_RETURN_KEY_MODE
    // 启动超时完成（无回车模式）
    flushTimer.start(FLUSH_TIMEOUT);
#endif

    return false;       // 不拦截
}
