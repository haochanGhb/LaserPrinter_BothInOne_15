#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    qRegisterMetaType<QVector<int>>("QVector<int>");

    m_pMarkHandle = new MarkHandle();
    m_pRemoteRecv = new RemoteRecv();
    m_pRemoteComRecv  = new RemoteRecv();

    m_pRemoteSyncRecv  = new RemoteRecv();
    m_pRemoteSend = new RemoteSend();
    m_pRemoteScannerSend = new RemoteSend();

    m_pStateHandle = new StateHandle();

    m_pPicCrop = new QSimplePicCrop();

    // 初始化新增的类
    m_markController = new MarkController(this);
    m_taskScheduler = new TaskScheduler(this);
    m_printStateMachine = new PrintStateMachine(this);

    // 初始化状态检查定时器
    m_state_check_timer = new QTimer(this);

    set_layout();
    set_style_sheet();

    load_data();

    load_mark_args();
    load_mark_image_args();

    connect_signals_and_slots();

//    setup_expired_tasks_tasks_monitor();

    //preheat test
    //int preheat_time = settings.create_setting.laser_preheat_time_cbb;
    //laser_preheat(preheat_time);
    refreshTodayPrintNumber();
    /*
    //远程标刻端口初始化
    m_pRemoteRecv->initUdp(settings.remote_text_setting.remote_text_network_port_le.toUShort());

    m_pRemoteSyncRecv->initUdp(9990);

    //串口同步初始化
    foreach(const QSerialPortInfo &port_info, QSerialPortInfo::availablePorts())
    {
        if(port_info.description().contains("USB Serial Port"))
        {
            m_pRemoteComRecv->initSerial(port_info.portName());
            break;
        }
    }

    //远程扫描端口初始化
    m_pRemoteScannerSend->initUdp(settings.remote_scanner_setting.remote_scanner_network_port_le.toUShort());

    //远程同步端口初始化
    mRemoteSyncSend->initUdp(9990);
    */
}

MainWindow::~MainWindow()
{
    // 停止定时器
    if (m_state_check_timer)
    {
        m_state_check_timer->stop();
    }

    delete m_pMarkHandle;
    delete m_pRemoteRecv;
    delete m_pRemoteComRecv;
    delete m_pRemoteSyncRecv;
    delete m_pRemoteSend;
    delete m_pRemoteScannerSend;
    delete m_pStateHandle;
    delete m_pPicCrop;
}

PrintProcessSta MainWindow::getPrintStateMachineState() const
{
    if (m_printStateMachine)
    {
        return m_printStateMachine->getState();
    }
    return PrintProcessSta::READY;
}
