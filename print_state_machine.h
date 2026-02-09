#ifndef PRINT_STATE_MACHINE_H
#define PRINT_STATE_MACHINE_H

#include <QObject>
#include "shared_enums.h"

class PrintStateMachine : public QObject
{
    Q_OBJECT

public:
    explicit PrintStateMachine(QObject *parent = nullptr);
    ~PrintStateMachine();

    void setState(PrintProcessSta state);
    PrintProcessSta getState() const;
    void start();
    void stop();
    void reset();

 signals:
    void stateChanged(PrintProcessSta newState, PrintProcessSta oldState);
    void readyStateEntered();
    void startMarkStateEntered();
    void changeSlotStateEntered();
    void finishedStateEntered();
    void endedStateEntered();
    void exceptionStateEntered();

private:
    PrintProcessSta m_currentState;
    PrintProcessSta m_previousState;

    void transitionToState(PrintProcessSta newState);
};

#endif // PRINT_STATE_MACHINE_H