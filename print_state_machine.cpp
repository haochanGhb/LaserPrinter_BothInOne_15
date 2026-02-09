#include "print_state_machine.h"

PrintStateMachine::PrintStateMachine(QObject *parent) : QObject(parent), m_currentState(READY), m_previousState(READY)
{
}

PrintStateMachine::~PrintStateMachine()
{
}

void PrintStateMachine::setState(PrintProcessSta state)
{
    if (m_currentState != state) {
        transitionToState(state);
    }
}

PrintProcessSta PrintStateMachine::getState() const
{
    return m_currentState;
}

void PrintStateMachine::start()
{
    transitionToState(STARTMARK);
}

void PrintStateMachine::stop()
{
    transitionToState(READY);
}

void PrintStateMachine::reset()
{
    transitionToState(READY);
}

void PrintStateMachine::transitionToState(PrintProcessSta newState)
{
    if (m_currentState == newState)
    {
        return;
    }

    PrintProcessSta oldState = m_currentState;
    m_previousState = oldState;
    m_currentState = newState;

    emit stateChanged(newState, oldState);

    // 发送对应状态的进入信号
    switch (newState) {
    case READY:
        emit readyStateEntered();
        break;
    case STARTMARK:
        emit startMarkStateEntered();
        break;
    case CHANGESLOT:
        emit changeSlotStateEntered();
        break;
    case FINISHED:
        emit finishedStateEntered();
        break;
    case ENDED:
        emit endedStateEntered();
        break;
    case EXCEPTION:
        emit exceptionStateEntered();
        break;
    default:
        break;
    }
}
