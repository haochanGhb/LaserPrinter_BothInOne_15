#ifndef MARK_CONTROLLER_H
#define MARK_CONTROLLER_H

#include <QObject>
#include "MarkHandle.h"
#include <QList>
#include <QLabel>
#include "public_fun.h"

class MarkController : public QObject
{
    Q_OBJECT

public:
    explicit MarkController(QObject *parent = nullptr);
    ~MarkController();

    void setMarkHandle(MarkHandle *markHandle);
    bool startMark();
    bool stopMark();
    void setFanOn(int level);

    int startMarkPicBuildIn(quint8 boxIndex, QList<QLabel*> &textList, BarcodeInfo &bcInfo,
                           MarkHandle::Rotate rotate, Align_type align);
//    int startMarkPicBuildIn(quint8 boxIndex, QList<QLabel*> &textList, BarcodeInfo &bcInfo,
//                           MarkHandle::Rotate rotate, bool isAlternated, Align_type align);

signals:
    void markStarted();
    void markFinished();
    void markError(const QString &error);
    void markStateChanged(int stateNum, const QString &desc);
    void stopMarked();

private slots:
    void onMarkStateChanged(int stateNum, const QString &desc);
    void onMarkFinished();
    void onMarkAlreadyStopped();

private:
    MarkHandle *m_markHandle;
};

#endif // MARK_CONTROLLER_H
