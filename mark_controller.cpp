#include "mark_controller.h"

MarkController::MarkController(QObject *parent) : QObject(parent), m_markHandle(nullptr)
{
}

MarkController::~MarkController()
{
}

void MarkController::setMarkHandle(MarkHandle *markHandle)
{
    if (m_markHandle) {
        disconnect(m_markHandle, nullptr, this, nullptr);
    }

    m_markHandle = markHandle;

    if (m_markHandle) {
        connect(m_markHandle, &MarkHandle::signalStateReport, this, &MarkController::onMarkStateChanged);
        connect(m_markHandle, &MarkHandle::signalMarkFinished, this, &MarkController::onMarkFinished);
        connect(m_markHandle, &MarkHandle::signalAlreadyStopped, this, &MarkController::onMarkAlreadyStopped);
    }
}

bool MarkController::startMark()
{
    if (!m_markHandle) {
        emit markError("MarkHandle not set");
        return false;
    }

    emit markStarted();
    return true;
}

bool MarkController::stopMark()
{
    if (!m_markHandle) {
        emit markError("MarkHandle not set");
        return false;
    }

    emit stopMarked();
    return true;
}

void MarkController::setFanOn(int level)
{
    if (m_markHandle) {
        m_markHandle->setFanOn(level);
    }
}

void MarkController::onMarkStateChanged(int stateNum, const QString &desc)
{
    emit markStateChanged(stateNum, desc);
}

void MarkController::onMarkFinished()
{
    emit markFinished();
}

void MarkController::onMarkAlreadyStopped()
{
    qDebug() << "onMarkAlreadyStopped.emit markFinished()";
    //emit markFinished();
}

int MarkController::startMarkPicBuildIn(quint8 boxIndex, QList<QLabel*> &textList, BarcodeInfo &bcInfo,
                                          MarkHandle::Rotate rotate, Align_type align)
{
    if (!m_markHandle) {
        emit markError("MarkHandle not set");
        return -1;
    }

    emit markStarted();
    return m_markHandle->startMarkPicBuildIn(boxIndex, textList, bcInfo, rotate, align);
}

//int MarkController::startMarkPicBuildIn(quint8 boxIndex, QList<QLabel*> &textList, BarcodeInfo &bcInfo,
//                                          MarkHandle::Rotate rotate, bool isAlternated, Align_type align)
//{
//    if (!m_markHandle) {
//        emit markError("MarkHandle not set");
//        return -1;
//    }

//    emit markStarted();
//    return m_markHandle->startMarkPicBuildIn(boxIndex, textList, bcInfo, rotate, isAlternated, align);
//}
