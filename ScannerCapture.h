#ifndef SCANNERCAPTURE_H
#define SCANNERCAPTURE_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

//#define NO_RETURN_KEY_MODE

class ScannerCapture : public QObject
{
    Q_OBJECT
public:
    static ScannerCapture *instance();

    void start();
    void stop();


private:
    explicit ScannerCapture(QObject *parent = nullptr);

signals:
    void scanFinished(const QString &code);
    void keyEnterCaptured();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QString buffer;
    QElapsedTimer intervalTimer;
    QTimer flushTimer;
    bool enabled = false;

    const int MAX_INTERVAL_MS = 50;     // 字符间隔时间
    const int MIN_LENGTH      = 1;      // 最小连续字符数
    const int FLUSH_TIMEOUT   = 120;    // 清空和完成时间（用于非回车判断）
};

#endif // SCANNERCAPTURE_H
