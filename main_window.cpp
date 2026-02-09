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

//    setup_expired_tasks_monitor();

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
           qDebug() << port_info.portName();
           if(port_info.portName().compare("COM4") == 0)
           {
                m_pRemoteComRecv->initCom("COM4");
           }
    }
    */

    //打印处理线程 - 已注释掉，程序启动不需要这个后台线程
    /*
    if(!watcher.isRunning())
    {
        future = QtConcurrent::run(this, &MainWindow::print_process);
        watcher.setFuture(future);
    }
    */

    //加载翻译器
    SettingDialog::ChangeTranslator(settings.language_setting.language_cbb);
}

MainWindow::~MainWindow()
{

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        isDraggable = true;
        event->accept();
    }
    else
    {
        isDraggable = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && isDraggable)
    {
        //move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isDraggable = false;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (NULL != event)
    {
           switch (event->type())
           {
               // this event is send if a translator is loaded
               case QEvent::LanguageChange:
                   qDebug() << "case QEvent::LanguageChange" << endl;
                   retranslateUi();
                   break;
               default:
                   break;
           }
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
     if(settings.list_setting.list_default_show_cbb.compare("task") == 0)
     {
         list_tab_widget->setCurrentIndex(0);
     }
     if(settings.list_setting.list_default_show_cbb.compare("print") == 0)
     {
         list_tab_widget->setCurrentIndex(1);
     }

     // 先删除过期任务，再加载任务列表
     setup_expired_tasks_monitor();

     // 加载任务列表
     task_list_panel->loadTaskListFromFile_OnThread();
     print_list_panel->loadTaskListFromFile_OnThread();

     //进入主窗体后，延时6.0秒提醒
      QTimer::singleShot(6000, this, [this]() {
          checkForReminder();
      });
}

void MainWindow::retranslateUi()
{
   title_bar->refreshTitleBarText();
   refreshTodayPrintNumber();
   task_list_panel->setTitle(tr("任务列表"));
//   task_list_panel->setButtonText(QString("%1(&E)").arg(tr("打印")), QString("%1(&E)").arg(tr("打印")), QString("%1(&R)").arg(tr("删除")));
   task_list_panel->setHeaderText(tr("次数"), tr("信息"), tr("内容"));

   print_list_panel->setTitle(tr("打印列表"));
//   print_list_panel->setButtonText(QString("%1(&F)").arg(tr("开始")), QString("%1(&F)").arg(tr("暂停")), QString("%1(&G)").arg(tr("删除")));
   print_list_panel->setHeaderText(tr("状态"), tr("信息"), tr("内容"));

   if(list_auto_btn->isChecked())
   {
       setList_auto_btn(false);
   }
   else
   {
       setList_auto_btn(true);
   }

   list_view_btn->setText(tr("查看"));
   list_print_btn->setText(QString("%1(&E)").arg(tr("打印")));
   list_delete_btn->setText(QString("%1(&R)").arg(tr("删除")));

   list_tab_widget->setTabText(0,tr("任务列表"));
   list_tab_widget->setTabText(1,tr("打印列表"));

   machineSta Sta = initing;
   switch (print_process_sta)
   {
        case READY:
        case ENDED:
            Sta = ready;
        break;
        case STARTMARK:
        case CHANGESLOT:
        case FINISHED:
            Sta = printing;
        break;

        case EXCEPTION:
           Sta = error;
   }

   if(!m_Init_finished)
   {
       Sta = initing;
   }

   title_bar->setMachineSta(Sta);

   status_bar->refreshStatusBarText();

   translateDefaultTemp();

   // 控制任务列表加载时机：程序启动时不加载（由showEvent处理），语言切换时重新加载
   static bool isFirstLaunch = true;
   if (!isFirstLaunch) {
       // 非首次调用（如语言切换时），重新加载任务列表
       task_list_panel->loadTaskListFromFile_OnThread();
       print_list_panel->loadTaskListFromFile_OnThread();
   }
   // 设置为非首次调用
   isFirstLaunch = false;

}


void MainWindow::translateDefaultTemp()
{
    //翻译默认固定模板里的“病理号”、“小号”、“备注1”
   if(temp_attr.panel_attr[0].name.compare("病理号") == 0)
   {
       edit_panel->setInputPanelName(0, tr("病理号"));
   }
   if(temp_attr.panel_attr[1].name.compare("小号") == 0)
   {
       edit_panel->setInputPanelName(1, tr("小号"));
   }
   if(temp_attr.panel_attr[2].name.compare("备注1") == 0)
   {
       edit_panel->setInputPanelName(2, tr("备注1"));
   }
}


void MainWindow::set_layout()
{
    this->setFixedSize(800, 1280);

    //标题栏区域
    title_area = new QWidget(this);
    title_area->setGeometry(0, 0, 800, 50);

    //中部主区域
    main_area = new QWidget(this);
    main_area->setGeometry(0, 50, 800, 1180);

    //状态栏区域
    status_area = new QWidget(this);
    status_area->setGeometry(0, 1230, 800, 50);

    //标题栏区域内容
    title_bar = new TitleBar(title_area);

    //状态栏区域内容
    status_bar = new StatusBar(status_area);

    //编辑面板
    edit_panel = new EditPanel();


    //任务列表面板
    task_list_panel = new TaskListPanel(this);
    task_list_panel->setTitle(tr("任务列表"));
//    task_list_panel->setButtonText(QString("%1(&E)").arg(tr("打印")), QString("%1(&E)").arg(tr("打印")), QString("%1(&R)").arg(tr("删除")));
    task_list_panel->setHeaderText("T", tr("信息"), tr("内容"));
    task_list_panel->setPrintStateMachine(m_printStateMachine);

    //打印列表面板
    print_list_panel = new TaskListPanel(this);
    print_list_panel->setTitle(tr("打印列表"));
//    print_list_panel->setButtonText(QString("%1(&F)").arg(tr("开始")), QString("%1(&F)").arg(tr("暂停")), QString("%1(&G)").arg(tr("删除")));
    print_list_panel->setHeaderText("S", tr("信息"), tr("内容"));
    print_list_panel->setPrintStateMachine(m_printStateMachine);

    //列表操作面板
    list_operate_area = new QWidget();
    list_operate_area->setFixedWidth(778);
    list_btn_panel = new QWidget();
    list_btn_panel->setFixedSize(778,80);

    list_auto_btn = new QPushButton();
    list_auto_btn->setFixedSize(150,50);
    list_auto_btn->setCheckable(true);
    QIcon icon(":/image/start.png"); // 例如使用资源文件里的图标
    list_auto_btn->setIcon(icon);
    list_auto_btn->setIconSize(QSize(21, 21));

    list_view_btn = new QPushButton();
    list_view_btn->setFixedSize(150,50);
    list_print_btn = new QPushButton();
    list_print_btn->setFixedSize(150,50);
    list_delete_btn = new QPushButton();
    list_delete_btn->setFixedSize(150,50);
    list_delete_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击后焦点停留

    QHBoxLayout *list_btn_layout = new QHBoxLayout(list_btn_panel);
    list_btn_layout->setContentsMargins(0,0,38,10);
    list_btn_layout->setSpacing(10);
    list_btn_layout->addStretch();

    list_view_btn->hide();
    //list_btn_layout->addWidget(list_view_btn);
    list_btn_layout->addWidget(list_print_btn);
    list_btn_layout->addWidget(list_delete_btn);
    list_btn_layout->addWidget(list_auto_btn);
    list_btn_layout->addStretch();

    list_tab_widget = new QTabWidget();
    list_tab_widget->addTab(task_list_panel,tr("任务列表"));
    list_tab_widget->addTab(print_list_panel,tr("打印列表"));

    QVBoxLayout *list_area_layout = new QVBoxLayout(list_operate_area);
    list_area_layout->setContentsMargins(30,20,30,20);
    list_area_layout->setSpacing(0);
    list_area_layout->addWidget(list_btn_panel, 0, Qt::AlignHCenter);
    list_area_layout->addWidget(list_tab_widget, 0, Qt::AlignHCenter);

    QVBoxLayout *main_layout = new QVBoxLayout(main_area);
    main_layout->setContentsMargins(0,8,0,8);
    main_layout->setSpacing(8);

    main_layout->addWidget(edit_panel, 0, Qt::AlignHCenter);
    main_layout->addWidget(list_operate_area, 0, Qt::AlignHCenter);
    //main_layout->addWidget(task_list_panel, 0, Qt::AlignHCenter);
    //main_layout->addWidget(print_list_panel, 0, Qt::AlignHCenter);
}

void MainWindow::set_style_sheet()
{
    //this->setStyleSheet(SS_MainWidget);
    title_area->setStyleSheet(SS_title_bar);
    //main_area->setStyleSheet(SS_MainWidget);
    status_area->setStyleSheet(SS_status_area);

    list_operate_area->setStyleSheet(SS_input_part);
    list_tab_widget->setStyleSheet(SS_tabwidget_area);

    list_auto_btn->setStyleSheet(SS_create_btn);
    list_view_btn->setStyleSheet(SS_Normal_Btn);
    list_print_btn->setStyleSheet(SS_Normal_Btn);
    list_delete_btn->setStyleSheet(SS_Normal_Btn);
}

void MainWindow::init_communication_setting()
{
    bool remoteOnOff = settings.remote_text_setting.remote_text_print_btn;
    bool syncOnOff = settings.sync_print_setting.sync_print_btn;

    bool scanSendEnable = settings.remote_text_setting.remote_scan_send_enable_btn;

    //同步接收端口初始化
    m_pRemoteSyncRecv->initUdp(9990);

    //程扫描接收发现端口使能
    m_pScannerReceiverDiscovery->setDiscoveryEnabled(remoteOnOff);
    m_pScannerReceiverDiscovery->setDeviceName(settings.remote_text_setting.remote_network_name_le);
    m_pScannerReceiverDiscovery->setServicePort(settings.remote_text_setting.remote_text_network_port_le.toUShort());

    //远程标刻接收端口初始化
    if(remoteOnOff)
    {
        m_pRemoteRecv->initUdp(settings.remote_text_setting.remote_text_network_port_le.toUShort());

        //串口同步初始化
        foreach(const QSerialPortInfo &port_info, QSerialPortInfo::availablePorts())
        {
            qDebug() << port_info.portName();
            if(port_info.portName().compare("COM4") == 0)
            {
                m_pRemoteComRecv->initCom("COM4");
            }
        }

    }
    else
    {
//        m_pRemoteRecv->close();
        m_pRemoteRecv->initUdp(65535);
        m_pRemoteComRecv->close();
    }

    quint16 local_port = 8008;
    QString remote_ip = settings.sync_print_setting.sync_print_network_addr_le;
    //settings.sync_print_setting.sync_print_network_port_le.toUInt();
    quint16 remote_port = 9990;
    if(syncOnOff)
    {
        m_pRemoteSend->initUdp(local_port, remote_ip, remote_port);
    }
    else
    {
        //m_pRemoteSend->close();
        m_pRemoteSend->initUdp(local_port, remote_ip, 65535);
        isCloseRemoteSyncSend = true;
    }
    m_pRemoteSend->detectLinkState();

    if(scanSendEnable)
    {
       m_pRemoteScannerSend->initUdp(0
                                  ,settings.remote_text_setting.remote_scan_send_ip_le
                                  ,settings.remote_text_setting.remote_scan_send_port_le.toUShort());
    }

}


void MainWindow::initScannerRececiverDiscovery()
{
    quint16 servicePort = settings.remote_text_setting.remote_text_network_port_le.toUShort();
    bool enable = settings.remote_text_setting.remote_text_print_btn;

    m_pScannerReceiverDiscovery = new DeviceDiscovery(this);
    m_pScannerReceiverDiscovery->setDiscoveryEnabled(enable);

    if (!m_pScannerReceiverDiscovery->startDevice(PRINTER_DEVICE_DISCOVER_PORT, settings.remote_text_setting.remote_network_name_le, servicePort))
    {
        qDebug() << "Bind UDP failed";
    }

    connect(m_pScannerReceiverDiscovery, &DeviceDiscovery::discoveryRequest,
                     [](const QHostAddress &addr, quint16 port) {
        qDebug() << "Discovery from" << addr.toString() << port;
    });
}

void MainWindow::connect_signals_and_slots()
{
    //主菜单菜单选项和退出按钮信号
    connect(title_bar, &TitleBar::history_option_Triggered, this, &MainWindow::on_history_option_triggered);
    connect(title_bar, &TitleBar::template_option_Triggered, this, &MainWindow::on_template_option_triggered);
    connect(title_bar, &TitleBar::setting_option_Triggered, this, &MainWindow::on_setting_option_triggered);
    connect(title_bar, &TitleBar::maintenance_option_Triggered, this, &MainWindow::on_maintenance_option_triggered);
    connect(title_bar, &TitleBar::about_option_Triggered, this, &MainWindow::on_about_option_triggered);
    connect(title_bar, &TitleBar::quit_btn_Clicked, this, &MainWindow::on_quit_btn_clicked);
    connect(title_bar, &TitleBar::quit_option_Triggered, this, &MainWindow::on_quit_btn_clicked);

    // 任务列表和打印列表的信号连接
//    connect(this, &MainWindow::addTextListToTaskTableSignal, this, &MainWindow::on_addTextListToTaskTable);
//    connect(this, &MainWindow::addTaskListToPrintTableSignal, this, &MainWindow::on_addTaskListToPrintTable);

    //编辑面板按钮信号
    connect(edit_panel, &EditPanel::create_btn_Clicked, this, &MainWindow::on_edit_panel_create_btn_clicked);
    connect(edit_panel, &EditPanel::create_task_action_Triggered, this, &MainWindow::on_edit_panel_create_task_action_triggered);
    connect(edit_panel, &EditPanel::create_task_and_print_action_Triggered, this, &MainWindow::on_edit_panel_create_task_and_print_action_triggered);
    connect(edit_panel, &EditPanel::create_task_and_print_instant_action_Triggered, this, &MainWindow::on_edit_panel_create_task_and_print_instant_action_triggered);
    //connect(edit_panel, &EditPanel::template_selector_Changed, this, &MainWindow::on_edit_panel_template_selector_changed);
    connect(edit_panel, &EditPanel::create_task_from_file_action_Triggered, this, &MainWindow::on_edit_panel_create_task_from_file_action_Triggered);
    connect(edit_panel, &EditPanel::template_edit_btn_Clicked, this, &MainWindow::on_template_option_triggered);

    connect(edit_panel, &EditPanel::expand_box_btn_Clicked, this, [=]() {
//        if(settings.create_setting.tray_empty_confirm_needed_btn)
//        {
//            DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
//            msg_box->setContent(tr("请确认已清空托盘里的包埋盒"));
//            if(msg_box->exec() != QDialog::Accepted)
//            {
//                return;
//            }
//        }

        if (!GlobalClickGuard::isAllowedClick(edit_panel, 5000))
        {
            return;
        }

        setTrayButtonEnable(false);
        QTimer::singleShot(5000, this, [=]() {
            setTrayButtonEnable(true);
        });

        on_expand_box();
    });

    connect(edit_panel, &EditPanel::retract_box_btn_Clicked, this, [=]()
    {
        if(settings.create_setting.tray_empty_confirm_needed_btn)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
            msg_box->setContent(tr("请确认已清空托盘里的包埋盒"));
            if(msg_box->exec() != QDialog::Accepted)
            {
                return;
            }
        }

        if (!GlobalClickGuard::isAllowedClick(edit_panel, 5000))
        {
            return;
        }

        setTrayButtonEnable(false);
        QTimer::singleShot(5000, this, [=]() {
            setTrayButtonEnable(true);
        });

        retract_box_box();
    });

    //任务列表按钮信号
    connect(task_list_panel, &TaskListPanel::max_btn_Clicked, this, &MainWindow::on_task_list_btnMaxMin_clicked);
    //connect(task_list_panel, &TaskListPanel::action_btn_Clicked, this, &MainWindow::on_task_list_print_btn_clicked);
    connect(list_print_btn,&QPushButton::clicked,this,&MainWindow::on_task_list_print_btn_clicked);
    connect(task_list_panel, &TaskListPanel::task_content_list_Extracted, this, &MainWindow::on_task_list_content_extracted);

    //打印列表按钮信号
    connect(print_list_panel, &TaskListPanel::max_btn_Clicked, this, &MainWindow::on_print_list_btnMaxMin_clicked);

    connect(print_list_panel, &TaskListPanel::action_btn_Clicked, this, &MainWindow::slot_setStartButtonChecked);

//    connect(print_list_panel, &TaskListPanel::set_print_finished, this, &MainWindow::on_print_finished);

    //connect(list_auto_btn,&QPushButton::clicked,this,&MainWindow::on_print_list_start_btn_clicked);
    connect(list_auto_btn, &QPushButton::clicked, this, [=]() {
        if (!GlobalClickGuard::isAllowedClick(list_auto_btn, 3000)) {
            return;
        }

        list_auto_btn->setEnabled(false);
        QTimer::singleShot(3000, list_auto_btn, [=]() {
            list_auto_btn->setEnabled(true);
        });

        on_print_list_start_btn_clicked(list_auto_btn->isChecked());
        });

    connect(print_list_panel, &TaskListPanel::task_content_list_Extracted, this, &MainWindow::on_task_list_content_extracted);

    connect(list_delete_btn,&QPushButton::clicked,this,&MainWindow::on_list_delete_btn_clicked);

    //获取打印列表打印任务文本信号
    connect(task_list_panel,&TaskListPanel::signal_task_info,this,&MainWindow::on_get_task_list_info);
    connect(print_list_panel,&TaskListPanel::signal_task_info,this,&MainWindow::on_get_task_list_info);

    //获取三连点击时钟按钮信号
    connect(status_bar,&StatusBar::datetime_btn_multiple_Clicked,this,&MainWindow::on_maintenance_option_triggered);

    connect(status_bar,&StatusBar::signal_datetime_update,this,&MainWindow::slot_setfanoff);

    //退出程序处理
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::on_application_about_to_quit);

    //标刻模块信号
    m_pMarkHandle->start();
    connect(m_pMarkHandle,&MarkHandle::signalStateReport,this,&MainWindow::slot_mark_State);
    connect(m_pMarkHandle,&MarkHandle::signalMarkFinished,this,&MainWindow::slot_mark_FinishStart);
    connect(m_pMarkHandle,&MarkHandle::signalAlreadyStopped,this,&MainWindow::slot_mark_AlreadyStopped);
    connect(m_pMarkHandle,&MarkHandle::signalPreparedBoxReady,this,&MainWindow::slot_on_prepared_box_ready,Qt::QueuedConnection);

    connect(m_pMarkHandle,&MarkHandle::signalMcuVersion, this, &MainWindow::slotOnMcuVersion);
    //connect(m_pStateHandle,&StateHandle::signalEndPrintState, this, &MainWindow::slotOnEndPrintState);

    //设备状态上报模块信号
    m_pStateHandle->start();
    connect(m_pStateHandle,&StateHandle::signalMcuState,this,&MainWindow::slot_McuState);
    connect(m_pStateHandle,&StateHandle::signalMcuAlarm,this,&MainWindow::slot_McuAlarm);

    ScannerCapture::instance()->start();
    connect(ScannerCapture::instance(), &ScannerCapture::scanFinished, [=](const QString &code){
            qDebug() << "Scan data:" << code;
            bool scanSendEnable = settings.remote_text_setting.remote_scan_send_enable_btn;
            if(scanSendEnable)
               m_pRemoteScannerSend->addTextTask(1,QStringList(),code);
        });


    //快捷键Enter立即打印（用于扫码枪）
//    shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Enter), this);
//    connect(shortcut_enter, &QShortcut::activated, this, [=](){qDebug() << "shortcut_enter pressed";
    connect(ScannerCapture::instance(), &ScannerCapture::keyEnterCaptured, this, [=](){
        // 设置为扫描枪输入模式
        InputPanel::setIsScannerInput(true);
        if(edit_panel->isLastInputPanalFocus() || settings.print_setting.auto_print_after_scan_btn)
        {
            create_task(true, false);
        }
        else
        {
            edit_panel->setNextInputPanalFocus(false);
        }
        // 延迟重置扫描枪输入标志，确保焦点切换完成
        QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
    });

    //快捷键Return立即打印（用于扫码枪）同上
//    shortcut_return = new QShortcut(QKeySequence(Qt::Key_Return), this);
//    connect(shortcut_return, &QShortcut::activated, this, [=](){qDebug() << "shortcut_return pressed";

    //快捷键Tab
    shortcut_tab = new QShortcut(QKeySequence(Qt::Key_Tab), this);
    connect(shortcut_tab, &QShortcut::activated, this, [=](){
        InputPanel::setIsScannerInput(true);
        edit_panel->setNextInputBoxFocus(true);
        // 延迟重置扫描枪输入标志，确保焦点切换完成
        QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
    });

    //快捷键Ctrl+Enter光标移到第一个输入框
    shortcut_ctrl_enter = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(shortcut_ctrl_enter, &QShortcut::activated, this, [=](){
        InputPanel::setIsScannerInput(true);
        edit_panel->setFirstInputPanalFocus();
        // 延迟重置扫描枪输入标志，确保焦点切换完成
        QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
    });

    //快捷键上
    shortcut_up = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this);
    connect(shortcut_up, &QShortcut::activated, this, [=](){
        InputPanel::setIsScannerInput(true);
        edit_panel->incFocusInputBox();
        // 延迟重置扫描枪输入标志，确保焦点切换完成
        QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
    });

    //快捷键下
    shortcut_down = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this);
    connect(shortcut_down, &QShortcut::activated, this, [=](){
        InputPanel::setIsScannerInput(true);
        edit_panel->decFocusInputBox();
        // 延迟重置扫描枪输入标志，确保焦点切换完成
        QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
    });

    //网络打印
    connect(m_pRemoteRecv,&RemoteRecv::signalLinkState,this,&MainWindow::slot_RecvRemoteLinkState);
    connect(m_pRemoteRecv,&RemoteRecv::signalStateReport,this,&MainWindow::slot_RecvRemoteState);
    connect(m_pRemoteRecv,&RemoteRecv::signalRecvTextTask,this,&MainWindow::slot_RecvTextTask);
    connect(m_pRemoteRecv,&RemoteRecv::signalRecvPicTask,this,&MainWindow::slot_RecvPicTask);


    //串口打印
    connect(m_pRemoteComRecv,&RemoteRecv::signalLinkState,this,&MainWindow::slot_RecvRemoteComLinkState);
    connect(m_pRemoteComRecv,&RemoteRecv::signalStateReport,this,&MainWindow::slot_RecvRemoteState);
    connect(m_pRemoteComRecv,&RemoteRecv::signalRecvTextTask,this,&MainWindow::slot_RecvTextTask);
    connect(m_pRemoteComRecv,&RemoteRecv::signalRecvPicTask,this,&MainWindow::slot_RecvPicTask);

    //同步打印接收服务端
    connect(m_pRemoteSyncRecv,&RemoteRecv::signalLinkState,this,&MainWindow::slot_RecvRemoteSyncLinkState);
    connect(m_pRemoteSyncRecv,&RemoteRecv::signalStateReport,this,&MainWindow::slot_RecvRemoteState);
    connect(m_pRemoteSyncRecv,&RemoteRecv::signalRecvTextTask,this,&MainWindow::slot_RecvTextTask);
    connect(m_pRemoteSyncRecv,&RemoteRecv::signalRecvPicTask,this,&MainWindow::slot_RecvPicTask);

    //同步打印
    setup_remoteSend_connection();

    connect(this, &MainWindow::allowDialogShow, this, [this](const QString &desc,int level) {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(desc);
        if(level < 2)
        {
            title_bar->setMachineSta(error);
        }
        msg_box->Exec(level,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle);

        title_bar->setMachineSta(ready);
        delete msg_box;
    });

    connect(list_tab_widget, &QTabWidget::currentChanged,this, &MainWindow::slot_list_tab_widget_currentChanged);

    title_bar->setMachineSta(initing);

    // 连接新增类的信号槽
    // 设置MarkHandle到MarkController
    m_markController->setMarkHandle(m_pMarkHandle);

    // 连接MarkController的信号
//    connect(m_markController, &MarkController::markStarted, this, &MainWindow::handleMarkStarted);
//    connect(m_markController, &MarkController::markFinished, this, &MainWindow::handleMarkFinished);
//    connect(m_markController, &MarkController::markError, this, [this](const QString &error) {
//        qDebug() << "Mark error:" << error;
//        handle_task_exception();
//    });
//    connect(m_markController, &MarkController::markStateChanged, this, &MainWindow::handleMarkStateChanged);
    connect(m_markController, &MarkController::stopMarked, this, &MainWindow::handle_task_ended_signal);

    // 设置TaskListPanel到TaskScheduler
    m_taskScheduler->setTaskListPanel(print_list_panel);

    // 连接TaskScheduler的信号
    connect(m_taskScheduler, &TaskScheduler::taskReady, this, &MainWindow::handleTaskReady);
    connect(m_taskScheduler, &TaskScheduler::taskQueueEmpty, this, &MainWindow::handle_task_ended);

    // 连接PrintStateMachine的信号，使用QueuedConnection避免锁竞争
    connect(m_printStateMachine, &PrintStateMachine::readyStateEntered, this, &MainWindow::handle_ready_state_entered, Qt::QueuedConnection);
    connect(m_printStateMachine, &PrintStateMachine::startMarkStateEntered, this, [this]() {
        this->handle_start_mark_state_entered();
        this->handle_task_startMark();
    }, Qt::QueuedConnection);
    connect(m_printStateMachine, &PrintStateMachine::changeSlotStateEntered, this, [this]() {
        this->handle_change_slot_state_entered();
        this->handle_task_changeSlot();
    }, Qt::QueuedConnection);
    connect(m_printStateMachine, &PrintStateMachine::finishedStateEntered, this, [this]() {
        this->handle_finished_state_entered();
        this->handle_task_finished();
    }, Qt::QueuedConnection);
    connect(m_printStateMachine, &PrintStateMachine::endedStateEntered, this, [this]() {
        this->handle_ended_state_entered();
        this->handle_task_ended();
    }, Qt::QueuedConnection);
    connect(m_printStateMachine, &PrintStateMachine::exceptionStateEntered, this, [this]() {
        this->handle_exception_state_entered();
        this->handle_task_exception();
    }, Qt::QueuedConnection);

    // 连接状态检查定时器
    connect(m_state_check_timer, &QTimer::timeout, this, &MainWindow::checkPrintState);
    m_state_check_timer->start(100); // 100ms检查一次状态
}

void MainWindow::slot_list_tab_widget_currentChanged(int index)
{
    if(0==index)
    {
        list_print_btn->setEnabled(true);
    }

    if(1==index)
    {
        list_print_btn->setEnabled(false);
    }
}

void MainWindow::setup_remoteRecv_connection()
{

}

void MainWindow::setup_remoteSend_connection()
{
     bool is_sync_print = settings.sync_print_setting.sync_print_network_rdb;

     if(!is_sync_print)
     {
         return;
     }

     connect(m_pRemoteSend, &RemoteSend::signalLinkState, this, [this](bool isOnline)
     {
         bool syncOnOff = settings.sync_print_setting.sync_print_btn;
         if(syncOnOff)
             status_bar->setSyncPrintLinkSta(isOnline);

         if(isCloseRemoteSyncSend)
         {
             isCloseRemoteSyncSend = false;
             status_bar->setSyncPrintLinkSta(false);
         }
     });
     connect(m_pRemoteSend, &RemoteSend::signalStateReport, this, [this](int stateNum, QString desc)
     {
         status_bar->setStatetext(QString("%1-%2").arg(stateNum).arg(desc));
     });
     connect(m_pRemoteSend, &RemoteSend::signalTaskSendFinish, this, [this](bool isOk)
     {
         qDebug() << "Receive send ack.";
         if(isOk)
         {
             QString task_name;
             //print_list_panel->setPrintingTaskFinished(task_name);
             task_list_panel->increaseTaskSta(task_name);
             //print_process_sta = PrintProcessSta::FINISHED;
             qDebug() << "Current task send finished.";
         }
         else
         {
             //print_list_panel->setPrintingTaskResend();
             //print_process_sta = PrintProcessSta::FINISHED;
             qDebug() << "Current task send failed, resend.";
         }
     });

     /*
     quint16 local_port = 8008;
     QString remote_ip = settings.sync_print_setting.sync_print_network_addr_le;
     //settings.sync_print_setting.sync_print_network_port_le.toUInt();
     quint16 remote_port = 9990;
     m_pRemoteSend->initUdp(local_port, remote_ip, remote_port);
     m_pRemoteSend->detectLinkState();
     */

     //初始化远程扫描接收发现端口
     initScannerRececiverDiscovery();

     init_communication_setting();

     //连接状态
     link_sta_detect_timer = new QTimer(this);
     connect(link_sta_detect_timer, &QTimer::timeout, this, [=]{
         m_pRemoteSend->detectLinkState();
         link_sta_detect_timer->start(3000);
     });
     link_sta_detect_timer->start(3000);
}

void MainWindow::load_data()
{
    SettingDialog::ReadSettings(&settings);

    //读取最后选择的模板名
    //edit_panel->read_last_selected_template_name();

    QStringList temp_names;
    temp_names.append(TemplateDialog::ReadTemplateNames(DEFAULT_TEMPLATES_DIR));
    temp_names.append(TemplateDialog::ReadTemplateNames(USER_TEMPLATES_DIR));
    connect(edit_panel, &EditPanel::template_selector_Changed, this, &MainWindow::on_edit_panel_template_selector_changed);

    edit_panel->setTemplateSelector(temp_names);
    edit_panel->setRepeatSelector(settings.create_setting.create_max_repeat_spb);

    task_list_panel->setIniFilePath(TASK_LIST_INI);
    task_list_panel->setIntervalSymbol(settings.list_setting.task_list_interval_symbol_le);

    print_list_panel->setIniFilePath(PRINT_LIST_INI);
    print_list_panel->setIntervalSymbol(settings.list_setting.print_list_interval_symbol_le);
    print_list_panel->setAllowDuplicatePrintTask(settings.print_setting.allow_duplicate_print_task_btn);

    //放在retranslateUi()里调用
//    task_list_panel->loadTaskListFromFile_OnThread();
//    print_list_panel->loadTaskListFromFile_OnThread();


    //回车即打印使能与否
    //shortcut_enter->setEnabled(settings.print_setting.auto_print_after_scan_btn);

    //扫码分割快捷键
    if(settings.print_setting.scan_code_separate_btn)
    {
        if(settings.print_setting.scan_delimiter_rdb)
        {
            edit_panel->setInputSeparateMode(1);
            edit_panel->setInputSeparateSymbol(settings.print_setting.scan_delimiter_le);
        }
        else if(settings.print_setting.scan_preset_text_length_rdb)
        {
            edit_panel->setInputSeparateMode(2);
            edit_panel->setInputPresetLengthStr(settings.print_setting.preset_text_length_le);
        }
    }
    else
    {
        edit_panel->setInputSeparateMode(0);//不可用
    }


    //设置输入连续符和分隔符
    edit_panel->setTypeinConnectorSymbol(settings.typein_setting.typein_connector_le);
    edit_panel->setTypeinDelimiterSymbol(settings.typein_setting.typein_delimiter_le);
    edit_panel->setMergeSymbol(settings.typein_setting.sub_number_merge_symbol_le);
    edit_panel->setPresetTextVisible(settings.typein_setting.preset_input_text_btn);

    //使能触摸键盘
    TouchKeyboardManager::setEnabled(settings.typein_setting.touch_keyboard_show_btn);
}

void MainWindow::load_mark_args()
{
    //文字标刻参数
    QFile file(MARK_LEVEL_ARGS_INI);
    QFileInfo info(file);

    if (false == info.exists())  // 创建默认配置
    {
        QSettings settings(MARK_LEVEL_ARGS_INI, QSettings::IniFormat);
        for (int i=0;i<10;i++)
        {
            settings.beginGroup(QString("PenArg_%1").arg(i+1));
            settings.setValue("markLoop", "1");
            settings.setValue("markSpeed", "1000");
            settings.setValue("powerRatio", "100");
            settings.setValue("current", "1");
            settings.setValue("freq", "40");
            settings.setValue("QPulseWidth", "20");
            settings.setValue("mopaPulse", "10");
            settings.setValue("startTC", "100");
            settings.setValue("offTC", "100");
            settings.setValue("endTC", "50");
            settings.setValue("polyTC", "80");
            settings.setValue("jumpSpeed", "8000");
            settings.setValue("jumpPosTC", "500");
            settings.setValue("jumpDistTC", "100");
            settings.setValue("endComp", "100");
            settings.setValue("pulsePointMode", "false");
            settings.setValue("pulseNum", "10");
            settings.setValue("pointTime", "0.1");
            settings.endGroup();

        }
    }

    QSettings settings(MARK_LEVEL_ARGS_INI, QSettings::IniFormat);
    for (int i=0;i<10;i++)
    {
        settings.beginGroup(QString("PenArg_%1").arg(i+1));
        m_mark_level_args[i].markLoop    = settings.value("markLoop").toInt();
        m_mark_level_args[i].markSpeed   = settings.value("markSpeed").toInt();
        m_mark_level_args[i].powerRatio  = settings.value("powerRatio").toInt();
        m_mark_level_args[i].current     = settings.value("current").toInt();
        m_mark_level_args[i].freq        = settings.value("freq").toInt();
        m_mark_level_args[i].QPulseWidth = settings.value("QPulseWidth").toInt();
        m_mark_level_args[i].mopaPulse   = settings.value("mopaPulse").toInt();
        m_mark_level_args[i].startTC     = settings.value("startTC").toInt();
        m_mark_level_args[i].offTC       = settings.value("offTC").toInt();
        m_mark_level_args[i].endTC       = settings.value("endTC").toInt();
        m_mark_level_args[i].polyTC      = settings.value("polyTC").toInt();
        m_mark_level_args[i].jumpSpeed   = settings.value("jumpSpeed").toInt();
        m_mark_level_args[i].jumpPosTC   = settings.value("jumpPosTC").toInt();
        m_mark_level_args[i].jumpDistTC  = settings.value("jumpDistTC").toInt();
        m_mark_level_args[i].endComp     = settings.value("endComp").toInt();
        m_mark_level_args[i].pulsePointMode = settings.value("pulsePointMode").toBool();
        m_mark_level_args[i].pulseNum    = settings.value("pulseNum").toInt();
        m_mark_level_args[i].pointTime   = settings.value("pointTime").toFloat();
        settings.endGroup();
    }

}

void MainWindow::load_mark_image_args()
{
    //文字标刻参数
    QFile file(MARK_IMAGE_LEVEL_ARGS_INI);
    QFileInfo info(file);

    if (false == info.exists())  // 创建默认配置
    {
        QSettings settings(MARK_IMAGE_LEVEL_ARGS_INI, QSettings::IniFormat);
        for (int i=0;i<10;i++)
        {
            settings.beginGroup(QString("PenArg_%1").arg(i+1));
            settings.setValue("markLoop", "1");
            settings.setValue("markSpeed", "300");
            settings.setValue("powerRatio", "100");
            settings.setValue("current", "1");
            settings.setValue("freq", "40");
            settings.setValue("QPulseWidth", "20");
            settings.setValue("mopaPulse", "10");
            settings.setValue("startTC", "100");
            settings.setValue("offTC", "100");
            settings.setValue("endTC", "50");
            settings.setValue("polyTC", "80");
            settings.setValue("jumpSpeed", "8000");
            settings.setValue("jumpPosTC", "500");
            settings.setValue("jumpDistTC", "100");
            settings.setValue("endComp", "100");
            settings.setValue("pulsePointMode", "false");
            settings.setValue("pulseNum", "10");
            settings.setValue("pointTime", "0.1");
            settings.endGroup();

        }
    }

    QSettings settings(MARK_IMAGE_LEVEL_ARGS_INI, QSettings::IniFormat);
    for (int i=0;i<10;i++)
    {
        settings.beginGroup(QString("PenArg_%1").arg(i+1));
        m_mark_image_level_args[i].markLoop    = settings.value("markLoop").toInt();
        m_mark_image_level_args[i].markSpeed   = settings.value("markSpeed").toInt();
        m_mark_image_level_args[i].powerRatio  = settings.value("powerRatio").toInt();
        m_mark_image_level_args[i].current     = settings.value("current").toInt();
        m_mark_image_level_args[i].freq        = settings.value("freq").toInt();
        m_mark_image_level_args[i].QPulseWidth = settings.value("QPulseWidth").toInt();
        m_mark_image_level_args[i].mopaPulse   = settings.value("mopaPulse").toInt();
        m_mark_image_level_args[i].startTC     = settings.value("startTC").toInt();
        m_mark_image_level_args[i].offTC       = settings.value("offTC").toInt();
        m_mark_image_level_args[i].endTC       = settings.value("endTC").toInt();
        m_mark_image_level_args[i].polyTC      = settings.value("polyTC").toInt();
        m_mark_image_level_args[i].jumpSpeed   = settings.value("jumpSpeed").toInt();
        m_mark_image_level_args[i].jumpPosTC   = settings.value("jumpPosTC").toInt();
        m_mark_image_level_args[i].jumpDistTC  = settings.value("jumpDistTC").toInt();
        m_mark_image_level_args[i].endComp     = settings.value("endComp").toInt();
        m_mark_image_level_args[i].pulsePointMode = settings.value("pulsePointMode").toBool();
        m_mark_image_level_args[i].pulseNum    = settings.value("pulseNum").toInt();
        m_mark_image_level_args[i].pointTime   = settings.value("pointTime").toFloat();
        settings.endGroup();
    }
}


void MainWindow::laser_preheat(int preheat_time)
{
    preheat_cnt = preheat_time;
    //倒计时
    QTimer *preheat_timer = new QTimer(this);
    connect(preheat_timer, &QTimer::timeout, this, [=]{
        if(preheat_cnt > 0)
        {
            preheat_cnt--;
            edit_panel->setLaserInfoText(tr("激光预热"), QString("%1").arg(preheat_cnt));
            preheat_timer->start(1000);


            //标志激光状态为预热中
        }
        else
        {
            edit_panel->setLaserInfoText(tr("激光预热"), QString("%1").arg(tr("完成")));
            //标志激光状态为预热完成
        }
    });
    preheat_timer->start(1000);
}

void MainWindow::create_task(bool auto_add_print_list, bool print_instant)
{
    //检查创建数量
    int repeat_times = edit_panel->GetRepeatTimes();
//    int text_list_cnt = edit_panel->GetTextListCount();
//    int task_list_cnt = task_list_panel->getListCount();

    //if((text_list_cnt + task_list_cnt) * repeat_times > MAX_TASKS)
//    if((text_list_cnt * repeat_times) + task_list_cnt > MAX_TASKS)
//    {
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(QString(tr("任务列表/打印列表允许的最大任务数为%1!")).arg(MAX_TASKS));
//        msg_box->exec();
//        return;
//    }

    //检查是否选择了槽盒
    if(edit_panel->GetSelectSlots().size() == 0)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("创建任务失败!\n请选择与任务相对应的槽盒号!"));
        msg_box->exec();
        return;
    }

    //获取输入内容列表
    if(edit_panel->GetTextListCount() > MAX_TASKS)
    {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(QString("%1%2").arg(tr("超过任务列表最大任务数")).arg(MAX_TASKS));
            msg_box->exec();
            return;
   }


    QStringList text_list = edit_panel->GetTextList();
    qDebug() << "获取输入内容列表:" << text_list;

    //重复及排列处理
    if(settings.create_setting.create_repeat_mode_cbb == "123..123..")
    {//123..123..
        QStringList str_list = text_list;
        for (int i=0; i<repeat_times-1; i++)
        {
            text_list.append(str_list);
        }
    }
    else
    {//11..22..33..
        for (int i=text_list.size()-1; i>=0; i--)
        {
            QString str = text_list[i];
            for (int j=0; j<repeat_times-1; j++)
            {
                text_list.insert(i, str);
            }
        }
    }

    //加入任务列表 - 发射信号
    QStringList add_to_print_list = task_list_panel->addTextListToTaskTable(text_list, TaskType::LOCAL_TASK);

    if(auto_add_print_list)
    {
        //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList(), print_instant);
        print_list_panel->addTaskListToPrintTable(add_to_print_list, print_instant);
        if(list_tab_widget->currentIndex() < 1)
        {
            list_tab_widget->setCurrentIndex(1);
        }
    }
    else
    {
        list_tab_widget->setCurrentIndex(0);
    }
}

bool MainWindow::read_taskTemplate_param(const QString &task_tempName, TemplateAttr &task_tempAttr)
{
    QStringList default_temp_names = TemplateDialog::ReadTemplateNames(DEFAULT_TEMPLATES_DIR);

    bool ret = false;
    //读取模板配置
    if(default_temp_names.contains(task_tempName))
    {//固定模板
        ret = TemplateDialog::ReadTemplateParam(DEFAULT_TEMPLATES_DIR, task_tempName, &task_tempAttr);
    }
    else
    {//自定义模板
        ret = TemplateDialog::ReadTemplateParam(USER_TEMPLATES_DIR, task_tempName, &task_tempAttr);
    }
    return ret;
}

void MainWindow::load_template_attr(const QString &tempName)
{
    QString selected_name = tempName;
    QStringList default_temp_names = TemplateDialog::ReadTemplateNames(DEFAULT_TEMPLATES_DIR);

    //读取模板配置
    if(default_temp_names.contains(selected_name))
    {//固定模板
        TemplateDialog::ReadTemplateParam(DEFAULT_TEMPLATES_DIR, selected_name, &temp_attr);
    }
    else
    {//自定义模板
        TemplateDialog::ReadTemplateParam(USER_TEMPLATES_DIR, selected_name, &temp_attr);
    }

    //设置输入框可见
    for (int i=0; i<MAX_INPUT_PANEL - 1; i++)
    {
        edit_panel->setInputPanelName(i, temp_attr.panel_attr[i].name);

        if((temp_attr.panel_attr[i].sta == "editable")
                ||
                (temp_attr.panel_attr[i].sta == "invisible")
                )
        {
            edit_panel->setInputPanelVisible(i, true);
        }
        else
        {
            edit_panel->setInputPanelVisible(i, false);
        }
    }

    edit_panel->updateInputPanelLayout();

    translateDefaultTemp();
    edit_panel->setCurrentTempAttr(temp_attr);

}

QString MainWindow::make_task_text_data_from_txt(const QString &remote_text)
{
    QString remote_content = remote_text;
    // 去除换行符和回车符
    remote_content.replace("\r\n", ""); // 先处理Windows风格的换行符
    remote_content.replace("\r", "");   // 处理单独的回车符
    remote_content.replace("\n", "");   // 处理单独的换行符

    QString print_front_symbol = settings.remote_text_setting.print_front_symbol_le;
    QString print_seprate_symbol = settings.remote_text_setting.print_delimiter_le;
    QString print_tail_symbol = settings.remote_text_setting.print_tail_symbol_le;
    QString slots_front_symbol = settings.remote_text_setting.slots_front_symbol_le;
    QString slots_seprate_symbol = settings.remote_text_setting.slots_delimiter_le;
    QString slots_tail_symbol = settings.remote_text_setting.slots_tail_symbol_le;

    //槽盒号和打印内容分割
    QString remote_slots_part;
    QString remote_print_part;
    if(!slots_front_symbol.isEmpty() && !slots_tail_symbol.isEmpty())
    {
        if(remote_content.contains(slots_front_symbol) && remote_content.contains(slots_tail_symbol))
        {
            remote_slots_part = remote_content.split(slots_front_symbol)[1].split(slots_tail_symbol)[0];
            remote_print_part.append(remote_content.split(slots_front_symbol)[0]);
            remote_print_part.append(remote_content.split(slots_tail_symbol)[1]);
        }
        else
        {
            remote_print_part = remote_content;
        }
    }
    else
    {
        remote_print_part = remote_content;
    }

    if(!print_front_symbol.isEmpty() && !print_tail_symbol.isEmpty())
    {//如果有分隔符，去掉分隔符
        if(remote_print_part.contains(print_front_symbol) && remote_print_part.contains(print_tail_symbol))
        {
            remote_print_part = remote_print_part.split(print_front_symbol)[1].split(print_tail_symbol)[0];
        }
    }

    qDebug() << "remote_slots_part:"<< remote_slots_part;
    qDebug() << "remote_print_part:"<< remote_print_part;

    //槽盒号识别
    QStringList slots_list;
    slots_list = remote_slots_part.split(slots_seprate_symbol);
    for (int i=slots_list.size()-1; i>=0; i--)
    {//去非正整数、去空
        bool is_num;
        slots_list[i].toUInt(&is_num);

        if(!is_num || slots_list[i].isEmpty())
        {
            slots_list.removeAt(i);
        }
    }
    qDebug() << "slots_list:"<< slots_list;

    //打印字段识别
    QStringList print_str_list;
    print_str_list = remote_print_part.split(print_seprate_symbol);
    qDebug() << "text_str_list:"<< print_str_list;

    //任务内容创建和填写
    QStringList task_str_list;
    for (int i=0; i<MAX_INPUT_PANEL+2; i++)
    {
        task_str_list.append(QString());
        if(i < print_str_list.size() && i < MAX_INPUT_PANEL)
        {
            //填写打印字段
            task_str_list[i] = print_str_list[i];
        }
    }
    //填写槽盒字段
    task_str_list[TaskTextPos::TASK_SLOTS] = (slots_list.join(","));

    QString task_text;
    task_text = task_str_list.join(SPLIT_SYMBOL);
    qDebug() << "task_text:"<< task_text;

    return task_text;
}


void MainWindow::on_history_option_triggered()
{
//    if(list_auto_btn->isChecked())
//    {
//        QString message = QString(tr("请先暂停打印后再进行统计查询操作！"));
//        emit allowDialogShow(message);
//        return;
//    }

    qDebug() << "history_option_triggered";
    HistoryDialog * history_dialog = new HistoryDialog();
    history_dialog->Exec();
    history_dialog->deleteLater();

    refreshTodayPrintNumber();
}

void MainWindow::on_template_option_triggered()
{
    DialogMsgBox *msg_box = new DialogMsgBox(INPUT_DIALOG);
    msg_box->setTitle(tr("模板"));
    msg_box->setInputTips(tr("访问密码："));
    QString passwd;
    if(msg_box->exec() == QDialog::Accepted)
    {
        passwd = msg_box->getInputText();

        if(passwd == QDateTime::currentDateTime().toString("HHmm"))
        {
            TemplateDialog *temp_dialog = new TemplateDialog();
            temp_dialog->setMarkHandle(m_pMarkHandle);
            temp_dialog->Exec();
            temp_dialog->deleteLater();

            //刷新模板列表
            QStringList temp_names;
            temp_names.append(TemplateDialog::ReadTemplateNames(DEFAULT_TEMPLATES_DIR));
            temp_names.append(TemplateDialog::ReadTemplateNames(USER_TEMPLATES_DIR));
            edit_panel->setTemplateSelector(temp_names);
        }
        else
        {
            DialogMsgBox *msg_box_info = new DialogMsgBox(INFO_DIALOG);
            msg_box_info->setContent(tr("密码错误！"));
            msg_box_info->exec();
        }
    }
}

void MainWindow::on_setting_option_triggered()
{
    if(!AUTO_TEST)
    {
        SettingDialog *setting_dialog = new SettingDialog();

        //恢复出厂设置，需要连接退出队列信号
        connect(setting_dialog,&SettingDialog::signale_print_task_exit_request,this,&MainWindow::on_print_task_exit_request);
        setting_dialog->exec();
        setting_dialog->deleteLater();

        //刷新配置信息
        SettingDialog::ReadSettings(&settings);
        edit_panel->setRepeatSelector(settings.create_setting.create_max_repeat_spb);
        task_list_panel->setIntervalSymbol(settings.list_setting.task_list_interval_symbol_le);
        print_list_panel->setIntervalSymbol(settings.list_setting.print_list_interval_symbol_le);
        print_list_panel->setAllowDuplicatePrintTask(settings.print_setting.allow_duplicate_print_task_btn);

        //回车即打印使能与否
        //shortcut_enter->setEnabled(settings.print_setting.auto_print_after_scan_btn);

        //扫码分割快捷键
        if(settings.print_setting.scan_code_separate_btn)
        {
            if(settings.print_setting.scan_delimiter_rdb)
            {
                edit_panel->setInputSeparateMode(1);
                edit_panel->setInputSeparateSymbol(settings.print_setting.scan_delimiter_le);
            }
            else if(settings.print_setting.scan_preset_text_length_rdb)
            {
                edit_panel->setInputSeparateMode(2);
                edit_panel->setInputPresetLengthStr(settings.print_setting.preset_text_length_le);
            }
        }
        else
        {
            edit_panel->setInputSeparateMode(0);//不可用
        }

        //设置输入连续符和分隔符
        edit_panel->setTypeinConnectorSymbol(settings.typein_setting.typein_connector_le);
        edit_panel->setTypeinDelimiterSymbol(settings.typein_setting.typein_delimiter_le);
        edit_panel->setMergeSymbol(settings.typein_setting.sub_number_merge_symbol_le);
        edit_panel->setPresetTextVisible(settings.typein_setting.preset_input_text_btn);

        //远程标刻端口初始化
        //m_pRemoteRecv->initUdp(settings.remote_text_setting.remote_text_network_port_le.toUShort());
        //所有网络参数配置重新初始化
        init_communication_setting();
    }
    else if(AUTO_TEST)
    {
        remote_init_test_timer = new QTimer(this);

        connect(remote_init_test_timer, &QTimer::timeout, this, [=]
        {
            SettingDialog *setting_dialog = new SettingDialog();
            //连接退出队列信号
            connect(setting_dialog,&SettingDialog::signale_print_task_exit_request,this,&MainWindow::on_print_task_exit_request);
            //setting_dialog->exec();
            QThread::msleep(20);
            setting_dialog->deleteLater();

            //刷新配置信息
            //SettingDialog::ReadSettings(&settings);
            edit_panel->setRepeatSelector(settings.create_setting.create_max_repeat_spb);
            task_list_panel->setIntervalSymbol(settings.list_setting.task_list_interval_symbol_le);
            print_list_panel->setIntervalSymbol(settings.list_setting.print_list_interval_symbol_le);
            print_list_panel->setAllowDuplicatePrintTask(settings.print_setting.allow_duplicate_print_task_btn);

            //扫码分割快捷键
            if(settings.print_setting.scan_code_separate_btn)
            {
                if(settings.print_setting.scan_delimiter_rdb)
                {
                    edit_panel->setInputSeparateMode(1);
                    edit_panel->setInputSeparateSymbol(settings.print_setting.scan_delimiter_le);
                }
                else if(settings.print_setting.scan_preset_text_length_rdb)
                {
                    edit_panel->setInputSeparateMode(2);
                    edit_panel->setInputPresetLengthStr(settings.print_setting.preset_text_length_le);
                }
            }
            else
            {
                edit_panel->setInputSeparateMode(0);//不可用
            }

            //设置输入连续符和分隔符
            edit_panel->setTypeinConnectorSymbol(settings.typein_setting.typein_connector_le);
            edit_panel->setTypeinDelimiterSymbol(settings.typein_setting.typein_delimiter_le);
            edit_panel->setMergeSymbol(settings.typein_setting.sub_number_merge_symbol_le);
            edit_panel->setPresetTextVisible(settings.typein_setting.preset_input_text_btn);

            settings.remote_text_setting.remote_text_print_btn = !settings.remote_text_setting.remote_text_print_btn;
            qDebug() << "settings.remote_text_setting.remote_text_print_btn===================================>>>>>" << settings.remote_text_setting.remote_text_print_btn;

            init_communication_setting();
            remote_init_test_timer->start(100);
        });

        remote_init_test_timer->start(100);
    }
}

void MainWindow::on_maintenance_option_triggered()
{
    DialogMsgBox *msg_box = new DialogMsgBox(INPUT_DIALOG);
    msg_box->setTitle(tr("调试"));
    msg_box->setInputTips(tr("访问密码："));
    QString passwd;
    if(msg_box->exec() == QDialog::Accepted)
    {
        passwd = msg_box->getInputText();

        if(passwd == QDateTime::currentDateTime().toString("HHmm"))
        {
            MaintenanceDialog *maintenance_dialog = new MaintenanceDialog();
            //恢复出厂设置，需要连接退出队列信号
            connect(maintenance_dialog,&MaintenanceDialog::signale_print_task_exit_request,this,&MainWindow::on_print_task_exit_request);

            maintenance_dialog->exec();
            maintenance_dialog->deleteLater();
            edit_panel->updateSlotColorMenuArgs();
            edit_panel->updateMarkPreviewTextAlign();

            MarkAreaArgs mark_area_args[2];
            for (int i = 0; i < 2; i++)
            {
                 MaintenanceDialog::ReadMarkAreaArgs(i, &mark_area_args[i]);

                 m_pMarkHandle->setMarkPosX(i,mark_area_args[i].x_offset_le);
                 m_pMarkHandle->setMarkPosY(i,mark_area_args[i].y_offset_le);
                 m_pMarkHandle->setMarkWidth(i,mark_area_args[i].x_width_le);
                 m_pMarkHandle->setMarkHeight(i,mark_area_args[i].y_height_le);
             }
        }
        else
        {
            DialogMsgBox *msg_box_info = new DialogMsgBox(INFO_DIALOG);
            msg_box_info->setContent(tr("密码错误！"));
            msg_box_info->exec();
        }
    }
}

void MainWindow::on_about_option_triggered()
{
    AboutDialog *about_dialog = new AboutDialog(this);
//    about_dialog->slotOnMcuManageVersion(m_mcuVersion);
    m_pMarkHandle->getVersion();
    about_dialog->exec();
    delete about_dialog;
    qDebug() << "about_option_triggered";
}

void MainWindow::on_quit_btn_clicked()
{
//    QMutexLocker locker(&m_mutex);

    /*
    QVector<PrintProcessSta> statusVector;
    statusVector << FINISHED << STOPWAIT << EXCEPTION << STARTMARK << CHANGESLOT;

    //获取标刻状态
    m_pStateHandle->getEndPrintState();
    QEventLoop eventloop;
    //接收到结束标刻信号，延长10ms后再退出
    connect(m_pStateHandle, &StateHandle::signalEndPrintState, &eventloop, [&eventloop]() {
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
    });
    //延时100ms，确保收到标刻结束状态
    QTimer::singleShot(100, &eventloop, SLOT(quit()));
    eventloop.exec();
  */

//    if(statusVector.contains(print_process_sta) &&  list_auto_btn->isChecked())
    if(list_auto_btn->isChecked() && print_list_panel->getListCount() > 0)
    {

        QString message = QString(tr("打印任务正在运行，请不要退出！"));
        emit allowDialogShow(message);
        return;
    }


//    if(list_auto_btn->isChecked() && print_list_panel->getListCount() > 0)
//    {
//        DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
//        msg_box->setContent(tr("打印任务正在运行，\n是否确定退出？"));
//        if (msg_box->exec() != QDialog::Accepted)
//        {
//            return;
//        }

//        setActionButtonReady();
//        slot_mark_AlreadyStopped();

//        QEventLoop eventloop;
//        //延时1000ms，确保收到标刻结束状态
//        QTimer::singleShot(1000, &eventloop, SLOT(quit()));
//        eventloop.exec();
//    }

    if(watcher.isRunning())
    {
        future.cancel();
        qDebug() << "on_quit_btn_clicked======>>>>>>退出线程";
        Logger::instance().log("on_quit_btn_clicked", "退出线程");
    }

    m_pMarkHandle->setFanOff();
    m_pMarkHandle->beepPlay(0);

    on_print_task_exit_request();

    edit_panel->clearInputPanelsFocus();
    this->clearFocus();
    //清除焦点后延时关闭键盘
    QTimer::singleShot(200, this, [this](){
        //隐藏触摸键盘
        TouchKeyboardManager::suppressTouchKeyboard(this);
        TouchKeyboardManager::hide();

    });

    //延时200毫秒后退出，给关闭风扇留有执行时间
    QTimer::singleShot(200, this, [this]()
    {
        QApplication::quit();
    });
}

void MainWindow::on_print_task_exit_request()
{
    qDebug() << "on_print_task_exit_request======>>>>>>退出队列";

    QTimer::singleShot(500, this, [this]()
    {
        on_quit_btn_clicked();
    });
}

void MainWindow::on_edit_panel_create_btn_clicked()
{
    bool is_auto_print = settings.print_setting.auto_print_after_task_established_btn;
    create_task(is_auto_print);
}

void MainWindow::on_edit_panel_create_task_action_triggered()
{
    //创建任务
    create_task();
}

void MainWindow::on_edit_panel_create_task_and_print_action_triggered()
{
    //创建任务、自动打印
    create_task(true);
}

void MainWindow::on_edit_panel_create_task_and_print_instant_action_triggered()
{
    //创建任务、自动打印、插队
    create_task(true, true);
}

void MainWindow::on_edit_panel_template_selector_changed(QString currentTemp)
{
    //load_template_attr(currentTemp);

    QTimer::singleShot(50, this, [this, currentTemp]() {
      load_template_attr_OnThread(currentTemp);
    });
}

void MainWindow::load_template_attr_OnThread(const QString &tempName)
{
    //线程加载数据
    QThread* thread = new QThread();
    connect(thread, &QThread::started, thread, [=]()mutable{
        qDebug()<<"started!";
        thread->quit();
    }, Qt::DirectConnection);
    connect(thread, &QThread::finished, this, [=]()mutable{
        thread->wait();
        thread->deleteLater();
        qDebug()<<"finished!";
        load_template_attr(tempName);
    });
    thread->start();
}

void MainWindow::on_edit_panel_create_task_from_file_action_Triggered()
{
    if(last_create_task_from_file_path == "")
    {
        last_create_task_from_file_path = QDir::homePath();
    }
    // 打开文件选择对话框（多选）
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                tr("选择文件"),
                last_create_task_from_file_path, // 默认路径
                tr("所有文件 (*.*);;文本文件 (*.txt);;CSV文件 (*.csv);;XLSX文件 (*.xlsx);;PDF文件 (*.pdf);;图片文件 (*.png *.jpg *.jpeg *.bmp)") // 文件类型过滤器
                );

    // 处理选择的文件路径
    if (!files.isEmpty())
    {
        last_create_task_from_file_path = files.first();

        for (const QString &file : files)
        {
            QFileInfo file_info(file);
            QString suffix = file_info.suffix().toLower();

            if(suffix == "txt" || suffix == "csv" || suffix == "xlsx")
            {// 文本任务
                qDebug() << "Importing text file:" << file;
                QStringList line_content_list;
                QStringList text_list;

                if(suffix == "xlsx")
                {
                    bool loadOk = true;
                    line_content_list =  getTaskDataListByXlsxParser(last_create_task_from_file_path, &loadOk);
                    if(line_content_list.size() > 0)
                    {
                        for(int i=0; i<line_content_list.size(); i++)
                        {
                            text_list.append(line_content_list[i]);
                        }
                    }
                }
                else
                {
                    if(get_text_from_file(file, line_content_list))
                    {
                        for(int i=0; i<line_content_list.size(); i++)
                        {
                            if(suffix == "txt")
                            {
                                text_list.append(make_task_text_data_from_txt(line_content_list[i]));
                            }
                            else if(suffix == "csv")
                            {
                                QString makeStr =  getTaskDataByCsvParser(line_content_list[i]);
                                //QString makeStr =  make_task_text_data_from_csv(line_content_list[i]);
                                if(!makeStr.isEmpty())
                                    text_list.append(makeStr);
                            }
                        }
                    }
                }

                //加入任务列表
                //task_list_panel->addTextListToTaskTable(text_list, TaskType::REMOTE_TEXT_TASK);
                QStringList add_to_print_list = task_list_panel->addTextListToTaskTable(text_list, TaskType::LOCAL_TASK);

                //添加到打印列表
                if(settings.print_setting.auto_print_after_task_established_btn)
                {
                    if(list_tab_widget->currentIndex() < 1)
                    {
                        list_tab_widget->setCurrentIndex(1);
                    }
                    //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList());
                    print_list_panel->addTaskListToPrintTable(add_to_print_list);

                }
            }
            else if(suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp")
            {// 图片任务
                qDebug() << "Importing image file:" << file;
                QImage png_image;
                QString png_name;
                //PNG图片处理
                if(png_image.load(file_info.absoluteFilePath()))
                {
                    png_name = file_info.fileName();

                    //加入任务列表
                    QStringList add_to_print_list = task_list_panel->addImageTaskToTaskTable(png_name, png_image);

                    //添加到打印列表
                    if(settings.print_setting.auto_print_after_task_established_btn)
                    {
                        if(list_tab_widget->currentIndex() < 1)
                        {
                            list_tab_widget->setCurrentIndex(1);
                        }
                        //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList());

                        if(add_to_print_list.size() > 0)
                        {
                           print_list_panel->addTaskListToPrintTable(add_to_print_list);
                        }
                    }
                }
                else
                {
                    qDebug() << "Invalid image file:" << file_info.absoluteFilePath();
                    DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
                    msg_box->setTitle(tr("提示"));
                    msg_box->setContent(QString("%1%2").arg(tr("无效图片文件："), file_info.absoluteFilePath()));
                    msg_box->exec();
                }
            }
            else if(suffix == "pdf")
            {// pdf导入的图片任务
                qDebug() << "Importing pdf file:" << file;
                QImage png_image;
                QString png_name;

                //PNG图片处理
                if(0 == m_pPicCrop->pdfToImage(file_info.absoluteFilePath(), png_image))
                {
                    png_name = file_info.fileName();

                    //加入任务列表
                    QStringList add_to_print_list = task_list_panel->addImageTaskToTaskTable(png_name, png_image);

                    //添加到打印列表
                    if(settings.print_setting.auto_print_after_task_established_btn)
                    {
                        if(list_tab_widget->currentIndex() < 1)
                        {
                            list_tab_widget->setCurrentIndex(1);
                        }
                        //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList());

                        if(add_to_print_list.size() > 0)
                        {
                           print_list_panel->addTaskListToPrintTable(add_to_print_list);
                        }
                    }
                }
                else
                {
                    qDebug() << "Invalid pdf file:" << file_info.absoluteFilePath();
                    DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
                    msg_box->setTitle(tr("提示"));
                    msg_box->setContent(QString("%1%2").arg(tr("无效的PDF文件："), file_info.absoluteFilePath()));
                    msg_box->exec();
                }
            }
        }
    }
}

void MainWindow::on_task_list_content_extracted(QStringList task_list)
{
    qDebug() << "on_task_list_content_extracted" << task_list;
    if(task_list.isEmpty())
    {
        return;
    }

    QStringList task_text_list = task_list.first().split(SPLIT_SYMBOL);
    if(task_text_list.size() < TaskTextPos::TASK_MAX_ITEM)
    {
        return;
    }

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        edit_panel->setInputPanelText(i , task_text_list[i]);
    }
}

void MainWindow::on_expand_box()
{
    QMutexLocker locker(&m_mutex);

    QVector<PrintProcessSta> statusVector;
    statusVector << FINISHED << EXCEPTION << STARTMARK << CHANGESLOT;

    if(statusVector.contains(print_process_sta) &&  list_auto_btn->isChecked())
    {
        QString message = QString(tr("打印任务正在运行,请勿移动托盘收纳盒！"));
        emit allowDialogShow(message);
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(message);
//        msg_box->exec();
        return;
    }

   if(1 == m_turn_motor_state)
   {
       QString message = QString(tr("托盘收纳盒正在移动，请勿重复操作！"));
       DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
       msg_box->setContent(message);
       msg_box->exec();
       return;
   }
   m_pMarkHandle->turnMotorSetRoute(14,0x00ffffff);
}

void MainWindow::retract_box_box()
{
    QMutexLocker locker(&m_mutex);

    QVector<PrintProcessSta> statusVector;
    statusVector << FINISHED << EXCEPTION << STARTMARK << CHANGESLOT;

    if(statusVector.contains(print_process_sta) &&  list_auto_btn->isChecked())
    {
        QString message = QString(tr("打印任务正在运行,请勿移动托盘收纳盒！"));
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(message);
//        msg_box->exec();
        emit allowDialogShow(message);
        return;
    }

    if(1 == m_turn_motor_state)
    {
        QString message = QString(tr("托盘收纳盒正在移动，请勿重复操作！"));
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(message);
//        msg_box->exec();
        emit allowDialogShow(message);
        return;
    }
    m_pMarkHandle->turnMotorSetRoute(0,0x00ffffff);
}

bool MainWindow::get_text_from_file(QString file_path, QStringList &content_list)
{
    QFileInfo file_info(file_path);
    QString suffix = file_info.suffix().toLower();
    if (suffix == "txt" || suffix == "csv")
    {
        QFile file(file_info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Cannot open txt file for reading:" << file_info.absoluteFilePath();
            return false;

        }


        QTextStream in(&file);
        //QString codec_name = "UTF-8";
        CodecSelectDialog *codecSelectDialog = new CodecSelectDialog();
        codecSelectDialog->setTargetFileName(file_info.fileName());
        if(codecSelectDialog->exec() != QDialog::Accepted)
            return false;
        QString codec = codecSelectDialog->getSelectedCodec();

        in.setCodec(QTextCodec::codecForName(codec.toUtf8()));

        while(!in.atEnd())
        {
            QString line = in.readLine();
            qDebug() << "readLine:" << line;
            // 去除换行符和回车符
            line.replace("\r\n", ""); // 先处理Windows风格的换行符
            line.replace("\r", "");   // 处理单独的回车符
            line.replace("\n", "");   // 处理单独的换行符

            content_list.append(line);
        }
        file.close();
        return true;
    }
    return false;
}

bool MainWindow::isExcelRowEmpty(const QStringList &row)
{
    for (const QString &cell : row) {
        if (!cell.trimmed().isEmpty()) {
            return false;  // 只要有一个有效内容，就不是空行
        }
    }
    return true;
}

QString MainWindow::make_task_text_data_from_csv(const QString &csv_line_content)
{
    //内容识别
    QStringList print_str_list;
    print_str_list = csv_line_content.split(",");

    // Skip empty row
    if(isExcelRowEmpty(print_str_list))
        return QString();

    //任务内容创建和填写
    QStringList task_str_list;
    for (int i=0; i<MAX_INPUT_PANEL+2; i++)
    {
        task_str_list.append(QString());
        if(i < print_str_list.size() && i < MAX_INPUT_PANEL)
        {
            //填写打印字段
            task_str_list[i] = print_str_list[i];
        }
    }

    QString task_text;
    task_text = task_str_list.join(SPLIT_SYMBOL);
    qDebug() << "task_text:"<< task_text;

    return task_text;
}


QString MainWindow::getTaskDataByCsvParser(const QString &srcText)
{
    //内容识别
    QStringList print_str_list;
    print_str_list = srcText.split(",");

    // Skip empty row
    if(isExcelRowEmpty(print_str_list))
        return QString();

    //任务内容创建和填写
    QStringList task_str_list;
    task_str_list.reserve(MAX_INPUT_PANEL + 1);

    for (int i = 0; i < MAX_INPUT_PANEL + 1; i++)
    {
        if(i < print_str_list.size())
        {
            task_str_list.append(print_str_list[i]);
        }
        else
        {
            task_str_list.append(QString());
        }
    }

    task_str_list.insert(15,QString());

    // Hopper字段处理，纯数字识别为槽盒号添加","分隔，否则为槽盒组名
    //（csv本以","分隔所以不支持以","分隔的槽盒号）
    bool isUInt;
    task_str_list[TaskTextPos::TASK_SLOTS].toUInt(&isUInt);
    if(isUInt)
    {// 纯数字
        task_str_list[TaskTextPos::TASK_SLOTS] = SUB_SPLIT_SYMBOL + task_str_list[TaskTextPos::TASK_SLOTS].split("").join(",");
    }
    else
    {// 非纯数字,识别为槽盒组名
        task_str_list[TaskTextPos::TASK_SLOTS] = task_str_list[TaskTextPos::TASK_SLOTS] + SUB_SPLIT_SYMBOL;
    }

    //合并索引17和18的元素（TASK_SLOTS和TASK_STA）
//    if (task_str_list.size() > 18) {
//        bool isPureNumber = false;

//        QString mergedStr = task_str_list[17];

//        // 调用函数处理合并后的字符串
//        QString processedStr = processSlotsString(mergedStr, isPureNumber);

//        if (isPureNumber) {
//            // 纯数字，使用处理后的字符串
//            task_str_list[17] = processedStr;
//        } else {
//            // 非纯数字，使用原规则
//            task_str_list[17] += task_str_list[18];
//        }
//        // 删除TASK_STA元素
//        task_str_list.removeAt(18);
//    }

    QString taskData;
    taskData = task_str_list.join(SPLIT_SYMBOL);
    qDebug() << "taskData:"<< taskData;

    return taskData;
}


QStringList MainWindow::getTaskDataListByXlsxParser(const QString &filePath, bool *loadOk)
{
    if(filePath.isEmpty())
        return QStringList();

    QStringList taskDataList;
    ExcelReader reader;

    if(reader.load(filePath))
    {
        if(loadOk) *loadOk = true;

        //任务内容创建和填写
        for(int i = 0; i < reader.rowCount(); ++i)
        {
            QStringList fieldList;
            for (int j = 0; j < MAX_INPUT_PANEL + 1; ++j)
            {
                fieldList.append(reader.cell(i, j));
            }

            qDebug() << "getTaskDataListByXlsxParser===" << fieldList.last();

            // Skip empty row
            if(isExcelRowEmpty(fieldList))
                continue;

            fieldList.insert(15,QString());

            // Hopper字段处理，纯数字识别为槽盒号添加“,”分隔，否则为槽盒组名
            if(!fieldList[TaskTextPos::TASK_SLOTS].contains(","))
            {
                bool isUInt;
                fieldList[TaskTextPos::TASK_SLOTS].toUInt(&isUInt);
                if(isUInt)
                {// 纯数字
                    fieldList[TaskTextPos::TASK_SLOTS] = fieldList[TaskTextPos::TASK_SLOTS].split("").join(",");
                }
                else
                {// 非纯数字，识别为槽盒组名
                    fieldList[TaskTextPos::TASK_SLOTS] = fieldList[TaskTextPos::TASK_SLOTS] + SUB_SPLIT_SYMBOL;
                }
            }

            //合并索引17和18的元素（TASK_SLOTS和TASK_STA）
//            if (fieldList.size() > 18) {
//                bool isPureNumber = false;
//                QString mergedStr = fieldList[17];

//                // 调用函数处理合并后的字符串
//                QString processedStr = processSlotsString(mergedStr, isPureNumber);

//                if (isPureNumber) {
//                    // 纯数字，使用处理后的字符串
//                    fieldList[17] = processedStr;
//                } else {
//                    // 非纯数字，使用原规则
//                    fieldList[17] += fieldList[18];
//                }

//                // 删除TASK_STA元素
//                fieldList.removeAt(18);
//            }
            taskDataList.append(fieldList.join(SPLIT_SYMBOL));
            qDebug() << fieldList;
         }
    }
    else
    {
        if(loadOk) *loadOk = false;
    }

    return taskDataList;
}





void MainWindow::on_task_list_print_btn_clicked()
{
    //检查创建数量
    int create_task_cnt = task_list_panel->getSelectedRowCnt();
    int print_list_cnt = print_list_panel->getListCount();

    //所选任务转换为打印任务
    QStringList task_list = task_list_panel->getSelectedTaskList();
    qDebug() << "on_task_list_print_btn_clicked.task_list=========="<< task_list;
    if(task_list.size() == 0)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选中需要打印的任务！"));
        msg_box->exec();
        return;
    }

    //设置当前显示列表为打印列表
    if(list_tab_widget->currentIndex() < 1)
    {
        list_tab_widget->setCurrentIndex(1);
    }
    print_list_panel->addTaskListToPrintTable(task_list);

}

void MainWindow::setList_auto_btn(bool isStart)
{
    QIcon icon; // 例如使用资源文件里的图标
    QPixmap pix;
   if(!isStart)
   {
       list_auto_btn->setText(QString("%1(&F)").arg(tr("暂停")));
       pix.load(":/image/pause.png");

   }
   else
   {
       list_auto_btn->setText(QString("%1(&F)").arg(tr("开始")));
       pix.load(":/image/start.png");

       //关掉风扇
       setFanOffDelay(settings.create_setting.gas_treatment_auto_sleep_cbb);
   }

   //设置托盘按钮与list_auto_btn按钮状态同步互斥
   setTrayButtonEnable(isStart);

   icon.addPixmap(pix);
   list_auto_btn->setIcon(icon);
}

void MainWindow::on_print_list_start_btn_clicked(bool isChecked)
{
//    print_list_panel->execActionButtonClicked();
    //设置当前显示列表为打印列表
    if(list_tab_widget->currentIndex() < 1)
    {
        list_tab_widget->setCurrentIndex(1);
        //不要在此加入打印任务队列，在TaskListPanel::loadTaskListFromFile()里加入
    }

    if(!isChecked)
    {
        qDebug() << "on_print_list_start_btn_clicked.checked2========>>>>>>" << list_auto_btn->isChecked();
        setList_auto_btn(true);

        // 禁用自动打印
        m_taskScheduler->setAutoPrintEnabled(false);

        // 设置MarkHandle为暂停状态，停止交替准备
        m_pMarkHandle->setPaused(true);

        if(AUTO_TEST)
        {
           return;
        }

        m_endPrintState = 2;
    }
    else
    {
        setList_auto_btn(false);

        qDebug() << "on_print_list_start_btn_clicked.checked1========>>>>>>" << list_auto_btn->isChecked();

        // 启用自动打印
        m_taskScheduler->setAutoPrintEnabled(true);

        // 设置MarkHandle为运行状态，允许交替准备
        m_pMarkHandle->setPaused(false);

        // 将状态设置为READY，以便定时器能够正确处理任务
        m_printStateMachine->setState(PrintProcessSta::READY);

        if(AUTO_TEST)
        {
           return;
        }
        //================================MCU_STATE==================================
        QStringList mcuStateNotifyKeyList ;
        mcuStateNotifyKeyList << "显示面板已合上" << "卸载滑轨已接好" << "载玻片物料盒是否已合上" << "组织盒(载玻片)是否已经堆满";
        for (const auto& item : m_nMcuStateDataList.getDataList())
        {
            qDebug() << "Key: " << item.key << ", Value: " << item.value;
            if(!mcuStateNotifyKeyList.contains(item.key))
            {
                continue;
            }
            bool ok;
            double value = item.value.toDouble(&ok);
            if (ok && value > 0)
            {
//                auto result = m_nMcuStateDataList.getMcuAliasByKey(item.key);
//                QString alias = result.first;
//                int level = result.second;

                McuAlertInfo info = m_nMcuStateDataList.getMcuAlertInfoByKey(item.key);
                QString alias = info.displayText;
                int level = info.soundLevel;
                QString code = info.errorCode;
                if(!code.isEmpty())
                {
                    alias = code + ": " + alias;
                }

                handle_task_exception();
                Logger::instance().log("on_print_list_start_btn_clicked", alias);
                emit allowDialogShow(alias,level);
                return;
            }
        }

        //================================MCU_STATE==================================

        if(1 == m_turn_motor_state)
        {
            QString message = QString(tr("托盘收纳盒正在移动，请勿启动打印任务！"));
            handle_task_exception();
            emit allowDialogShow(message,2);
            return;
        }

    }

}

void MainWindow::on_list_delete_btn_clicked()
{
    //QMutexLocker locker(&m_mutex);

//    QVector<PrintProcessSta> statusVector;
//    statusVector << FINISHED << STOPWAIT << EXCEPTION << STARTMARK << CHANGESLOT;
//    if(statusVector.contains(print_process_sta) &&  list_auto_btn->isChecked())
//    {
//        QString message = QString(tr("打印任务正在运行，请勿执行删除操作！"));
//        emit allowDialogShow(message);
//        return;
//    }

   if(list_tab_widget->currentIndex() < 1)
   {
        task_list_panel->execDeleteButtonClicked(false);
   }
   else
   {
        list_auto_btn->setEnabled(true);
        print_list_panel->execDeleteButtonClicked(list_auto_btn->isChecked());

        // 异常场景下恢复队列,插入/删除都要恢复
   }
}

bool MainWindow::isPrintListAutoMode(PrintProcessSta &print_process_sta)
{
    // 通过两个关卡防止打印时删除记录
    print_process_sta = this->print_process_sta;

    return list_auto_btn->isChecked() && (list_tab_widget->currentIndex() == 1);
}

QString MainWindow::getMcuVersion()
{
     return m_mcuVersion;
}

int MainWindow::getEndPrintState()
{
     qDebug() << QString("getEndPrintState============%1").arg(QString::number(m_endPrintState));
     return m_endPrintState;
}


/*
// 槽位字符串处理函数
QStringList MainWindow::processSlotStrings(const QString &slots_num_str)
{
    QStringList resultStrings;

    if (slots_num_str.isEmpty()) {
        return resultStrings;
    }

    // 以 "0x1f" 分割字符串
    QStringList parts = slots_num_str.split(SUB_SPLIT_SYMBOL);

    if (parts.size() == 1) {
        // 没有分隔符，直接解析为槽位数字
        QStringList slotNumbers = parts[0].split(",");
        for (const QString &numStr : slotNumbers) {
            bool isNumber = false;
            int slotNumber = numStr.trimmed().toInt(&isNumber);
            if (
                    isNumber && (slotNumber > 0)
                    && (slotNumber <= BUTTONS_PER_GROUP)
                    &&(!resultStrings.contains(QString::number(slotNumber)))
               )
            {
                resultStrings.append(QString::number(slotNumber));
            }
        }
        return resultStrings;
    }

    // 第一部分是关键字，第二部分是备选槽位
    QString keyword = parts[0].trimmed();
    QString fallbackSlots = parts[1].trimmed();

    // 获取所有映射符
    QStringList allMappings = SettingDialog::getAllMappings();

    // 检查关键字是否是有效的映射符
    if (allMappings.contains(keyword))
    {
        // 关键字是有效映射符，使用映射的槽位
        QString mappedSlots = SettingDialog::findSelectedSlotsByKeyword(keyword);
        if (!mappedSlots.isEmpty())
        {
            resultStrings = mappedSlots.split(",");
            qDebug() << "Using mapped slots for keyword" << keyword << ":" << resultStrings;
            return resultStrings;
        }
    }

    // 关键字无效或映射失败，使用备选槽位
    QStringList slotNumbers = fallbackSlots.split(",");

    for (const QString &numStr : slotNumbers) {
        bool isNumber = false;
        int slotNumber = numStr.trimmed().toInt(&isNumber);
        if (
                isNumber && (slotNumber > 0)
                && (slotNumber <= BUTTONS_PER_GROUP)
                &&(!resultStrings.contains(QString::number(slotNumber)))
           )
        {
            resultStrings.append(QString::number(slotNumber));
        }
    }

    qDebug() << "Using fallback slots:" << resultStrings;
    return resultStrings;
}
*/

QStringList MainWindow::processSlotStrings(const QString &slots_num_str)
{
    QStringList resultStrings;

    if (slots_num_str.isEmpty()) {
        return resultStrings;
    }

    // 以 "0x1f" 分割字符串
    QStringList parts = slots_num_str.split(SUB_SPLIT_SYMBOL);

    // 情况1：没有分隔符 → 直接解析数字串
    if (parts.size() == 1) {
        QStringList slotNumbers = parts[0].split(",");
        for (const QString &numStr : slotNumbers) {
            bool isNumber = false;
            int slotNumber = numStr.trimmed().toInt(&isNumber);
            if (isNumber && slotNumber > 0 && slotNumber <= BUTTONS_PER_GROUP) {
                resultStrings.append(QString::number(slotNumber));
            }
        }
        return resultStrings;
    }

    // 情况2：有分隔符
    QString keyword = parts[0].trimmed();
    QString fallbackSlots = parts[1].trimmed();

    if (!keyword.isEmpty()) {
        // 有关键字，优先解析关键字
        QString mappedSlots = SettingDialog::findSelectedSlotsByKeyword(keyword);
        if (!mappedSlots.isEmpty()) {
            resultStrings = mappedSlots.split(",");
            qDebug() << "Using mapped slots for keyword" << keyword << ":" << resultStrings;
            return resultStrings;
        } else {
            // 关键字无效或没有映射 → 返回空，不再 fallback
            qDebug() << "Keyword" << keyword << "not found, returning empty.";
            return resultStrings;
        }
    } else {
        // 没有关键字（形如 "0x1f1,3,5"） → 返回数字串
        QStringList slotNumbers = fallbackSlots.split(",");
        for (const QString &numStr : slotNumbers) {
            bool isNumber = false;
            int slotNumber = numStr.trimmed().toInt(&isNumber);
            if (isNumber && slotNumber > 0 && slotNumber <= BUTTONS_PER_GROUP) {
                resultStrings.append(QString::number(slotNumber));
            }
        }
        qDebug() << "Using slots (no keyword):" << resultStrings;
        return resultStrings;
    }
}

QString MainWindow::processSlotsString(const QString &slotsStr, bool &isPureNumber)
{
    // 检查是否为纯数字
    isPureNumber = true;
    for (QChar c : slotsStr) {
        if (!c.isDigit()) {
            isPureNumber = false;
            break;
        }
    }

    if (isPureNumber) {
        // 纯数字处理：拆分、排序、去重、范围限制(1-9)
        QSet<int> uniqueNumbers;
        for (QChar c : slotsStr) {
            int num = c.digitValue();
            if (num >= 1 && num <= 9) {
                uniqueNumbers.insert(num);
            }
        }

        // 转换为列表并排序
        QList<int> sortedNumbers = uniqueNumbers.toList();
        qSort(sortedNumbers);

        // 转换为以','分隔的字符串
        QStringList numberStrs;
        for (int num : sortedNumbers) {
            numberStrs << QString::number(num);
        }

        return numberStrs.join(",");
    } else {
        // 非纯数字，保持原字符串
        return slotsStr;
    }
}


void MainWindow::handle_task_ready(const QString& taskName)
{
    QMutexLocker locker(&m_mutex);

    QSettings task_list_ini(PRINT_LIST_INI, QSettings::IniFormat);
    task_list_ini.beginGroup("TaskList");
    QString task_str = task_list_ini.value(taskName, "").toString();
    task_list_ini.endGroup();

    if (task_str.isEmpty()) {
        Logger::instance().log("handle_task_ready", "未找到任务数据: " + taskName);
        //这里执行异常，会导致删除过程中异常中断
        //handle_task_exception();
        return;
    }

    //冗余赋值
    //current_print_name = taskName;
    current_print_task = task_str;

    QStringList task_text_split = task_str.split(SPLIT_SYMBOL);

    QString slots_num_str = task_text_split[TaskTextPos::TASK_SLOTS];
//    if (slots_num_str.isEmpty())
//    {
//        slots_num_str = edit_panel->GetSelectSlots(); // 默认槽位
//    }
    //QStringList slotStrings = slots_num_str.split(",");
    QStringList slotStrings = processSlotStrings(slots_num_str);
    qDebug() << "Processed slot strings==================>>>>>>" << slotStrings;

    //解析后无槽位，默认选择界面槽位
    if(slotStrings.size() < 1)
    {
        slots_num_str = edit_panel->GetSelectSlots();
        slotStrings = slots_num_str.split(",");
    }

    QString task_name = task_text_split[TaskTextPos::TASK_NAME];
    unsigned task_type = task_text_split[TaskTextPos::TASK_TYPE].toUShort();

    if (task_type == TaskType::REMOTE_IMAGE_TASK) {
        QString datePart = task_name.mid(5, 8);
        QString filePath = task_list_panel->getTaskdataDir() + datePart + "/";
        QString fullFileName = filePath + task_name + ".png";
        QImage image;
        image.load(fullFileName);
        m_pRemoteSend->addPicTask(1, slotStrings, image);
    } else {
        QString sync_print_text = make_remote_text_data(task_str);
        m_pRemoteSend->addTextTask(1, slotStrings, sync_print_text);
    }

    m_current_slots.clear();
    for (const QString& slot : slotStrings) {
        bool ok;
        int slotNum = slot.toInt(&ok);
        if (ok && slotNum > 0) {
            m_current_slots.push_back(slotNum);
        }
    }

    if (!m_current_slots.empty())
    {
        if(edit_panel->GetAlternatingPrint())
        {
            selectAlternatingSlot();
        }
        else
        {
            selectOptimalSlot();
        }

        print_list_panel->setPrintTaskState(current_print_name, PrintSta::PRINT_STA_PRINTING);
        m_printStateMachine->setState(PrintProcessSta::STARTMARK);
    }
    else
    {
        m_printStateMachine->setState(PrintProcessSta::EXCEPTION);
        Logger::instance().log("handle_task_ready", "任务没有设定槽盒号！");
        emit allowDialogShow(tr("任务没有设定槽盒号！"));
    }
}


void MainWindow::handle_task_changeSlot()
{
    // 使用互斥锁保护整个操作
     QMutexLocker locker(&m_mutex);

//     if (!print_list_panel->getPrintTask(current_print_name, current_print_task))
     if (print_list_panel->getListCount() < 1)
     {
         return;
     }

        qDebug() << "getPrintTask" << current_print_name << current_print_task;
        if(m_current_slots.size() > 0)
        {
            m_slots_current_index = m_current_slots[0];
            m_slots_last_index = m_slots_current_index;
            //转换成标刻调用的索引号，显示槽盒号-1
            m_slots_current_index -= 1;
            //弹出首个槽盒
            m_current_slots.erase(m_current_slots.begin());

            print_list_panel->setPrintTaskState(current_print_name, PrintSta::PRINT_STA_PRINTING);

            m_printStateMachine->setState(PrintProcessSta::STARTMARK);
        }
        else
        {
            m_printStateMachine->setState(PrintProcessSta::ENDED);
        }
}


void MainWindow::handle_task_startMark()
{

    qDebug() << "handle_task_startMark1,getState ==========" << m_printStateMachine->getState();
    //调用标刻接口
    if(m_mutex.tryLock(3000))  // 尝试等待最多3秒
    {
        m_markController->setFanOn(settings.create_setting.gas_treatment_speed_level_cbb);
        m_FanOn_Start = true;
        //延时20s关闭
        setFanOffDelay();
        // 调用完整的标刻流程
        qDebug() << "handle_task_startMark2,getState ==========" << m_printStateMachine->getState();
        startMark();
        qDebug() << "handle_task_startMark3,getState ==========" << m_printStateMachine->getState();
        m_mutex.unlock();
        qDebug() << "handle_task_startMark4,getState ==========" << m_printStateMachine->getState();
    }
    else
    {
         Logger::instance().log("handle_task_startMark()", "获取锁失败,标可资源被占用=======");
    }

        qDebug() << "handle_task_startMark5,getState ==========" << m_printStateMachine->getState();
}

void MainWindow::handle_task_finished()
{
    // 使用互斥锁保护整个操作
    QMutexLocker locker(&m_mutex);

    // 清除当前任务，允许调度新任务
    m_taskScheduler->setCurrentTask(QString());

    // 根据list_auto_btn状态和任务列表状态来决定是否切换到ENDED状态
    if (!list_auto_btn->isChecked() || print_list_panel->getListCount() < 1)
    {
        // list_auto_btn状态为false（已暂停），或者没有任务了，切换到ENDED状态
        m_printStateMachine->setState(PrintProcessSta::ENDED);
    }
    else
    {
        // list_auto_btn状态为true（正在运行），并且还有任务，切换到READY状态继续处理下一个任务
        m_printStateMachine->setState(PrintProcessSta::READY);
    }

    refreshTodayPrintNumber();

    qDebug() << "handle_task_finished,getState ==========" << m_printStateMachine->getState();
}

void MainWindow::handle_task_ended()
{
    // 清除当前任务，允许调度新任务
    m_taskScheduler->setCurrentTask(QString());

    m_printStateMachine->setState(PrintProcessSta::READY);

    //emit stopMark()
    m_markController->stopMark();
}

void MainWindow::handle_task_ended_signal()
{
    QMutexLocker locker(&m_mutex);
    bool alternatingPrint = edit_panel->GetAlternatingPrint();
    int print_row_cnt = print_list_panel->getListCount();

    if(alternatingPrint)
    {
        // 如果暂停了，且有提前准备好的槽盒，则不停止提前准备好的槽盒
        qDebug() << "handle_task_ended_signal: m_pMarkHandle->getPreparedBoxIndex()==="<< m_pMarkHandle->getPreparedBoxIndex();
        if(!list_auto_btn->isChecked() && m_pMarkHandle->getPreparedBoxIndex() != 0xFF)
        {
            qDebug() << "handle_task_ended_signal: 暂停状态，有提前准备好的槽盒，只停止当前槽盒";
            //m_pMarkHandle->stopMark(m_slots_current_index);
            //这里不要停止退出最后一个，而是继续完成最后一个任务
            //QString firstTask = print_list_panel->getFirstWaitingTaskNameFromModel();
            //handleTaskReady(firstTask);
        }
        else
        {
            m_pMarkHandle->stopMark((m_slots_current_index + 1) % 2);
            m_pMarkHandle->stopMark(m_slots_current_index);
        }
    }
    else
    {
        m_pMarkHandle->stopMark(m_slots_current_index);
    }
}

void MainWindow::handle_task_exception()
{
    Logger::instance().log("handle_task_exception", "========================");
    setActionButtonReady();

    // 清除当前任务，允许调度新任务
    m_taskScheduler->setCurrentTask(QString());

    //这里不要调用m_pMarkHandle->stopMark()，
    //导致循环调用handle_task_exception(),崩溃;

    slot_mark_AlreadyStopped();

    //关闭风扇不应放在异常处理过程，会引起死锁
    //m_pMarkHandle->setFanOff();

    //测试用
    //on_print_finished(current_print_name);
    //print_process_sta = PrintProcessSta::READY;

    //title_bar->setMachineSta(error);
    // 不再使用print_task_queue，改为使用TaskScheduler
    // print_task_queue.requestExit();
}

void MainWindow::handle_ready_state_entered()
{
    qDebug() << "handle_ready_state_entered: 进入READY状态";

    status_bar->setStatetext("结束任务");
    if(!m_Init_finished)
    {
        title_bar->setMachineSta(initing);
    }
    else
    {
        title_bar->setMachineSta(ready);
    }
}

void MainWindow::handle_start_mark_state_entered()
{
    qDebug() << "handle_start_mark_state_entered: 进入STARTMARK状态";
    title_bar->setMachineSta(printing);
}

void MainWindow::handle_change_slot_state_entered()
{
    qDebug() << "handle_change_slot_state_entered: 进入CHANGESLOT状态";
    title_bar->setMachineSta(printing);
}

void MainWindow::handle_finished_state_entered()
{
    qDebug() << "handle_finished_state_entered: 进入FINISHED状态";
    title_bar->setMachineSta(ready);
}

void MainWindow::handle_ended_state_entered()
{
    qDebug() << "handle_ended_state_entered: 进入ENDED状态";
    title_bar->setMachineSta(ready);
}

void MainWindow::handle_exception_state_entered()
{
    qDebug() << "handle_exception_state_entered: 进入EXCEPTION状态";
    title_bar->setMachineSta(error);
}

void MainWindow::slot_setStartButtonChecked(bool isChecked)
{
    //这个状态下恢复不了启动
    if(isChecked)
    {
        // 启用自动打印
//        m_taskScheduler->setAutoPrintEnabled(true);
    }

    list_auto_btn->setChecked(isChecked);
    setList_auto_btn(!isChecked);
}

void MainWindow::setTrayButtonEnable(bool isEnable)
{
       edit_panel->setExpand_box_btn_enble(isEnable);
       edit_panel->setRetract_box_btn_enble(isEnable);
}

void MainWindow::setActionButtonReady()
{

    //print_list_panel->setActionButtonReady();

    if(list_auto_btn->isChecked())
    {
        list_auto_btn->setChecked(false);
        //list_auto_btn->setText(tr("开始"));
        setList_auto_btn(true);

        //保证 UI 和线程状态联动？无需用这种翻转方式，不准确
        //QMetaObject::invokeMethod(list_auto_btn, "click", Qt::QueuedConnection);
    }

//      else
//      {

//            list_auto_btn->setChecked(true);
//            setList_auto_btn(false);

//      }
}

void MainWindow::setFanOffDelay(bool isSetTime)
{
    //延时关闭风扇
    //1次 = 1s,自然延时20s
    m_FanOff_delay_count = 20;

    if(isSetTime)
    {
        //设置延时
        m_FanOff_delay_count = settings.create_setting.gas_treatment_auto_sleep_cbb;
    }
    //不延时的情况
    if(m_FanOff_delay_count < 1)
    {
        m_FanOff_delay_count = 1;
    }

}

void MainWindow::setFanOffDelay(int iSec)
{
   m_FanOff_delay_count = iSec;

   //不延时的情况
   if(m_FanOff_delay_count < 1)
   {
       m_FanOff_delay_count = 1;
   }
}


// 打印处理线程函数 - 已注释掉，不再使用后台线程，改为使用定时器
/*
void MainWindow::print_process()
{
    while (true)
    {
        if(future.isCanceled())
        {
            qDebug() << "Print process thread canceled:" << QThread::currentThread();
            return ;
        }

//        if (print_task_queue.exitRequested()) {
//             qDebug() << "[print_process] 检测到退出请求，结束线程";
//             return;
//         }

        switch (print_process_sta)
        {
        case READY:
            if (list_auto_btn->isChecked())
            {
                if (!current_print_name.isEmpty())
                {
                    qDebug() << "[print_process] 开始处理任务：" << current_print_name;

                    // 注意：handle_task_ready() 内部必须根据成功/失败设置下一个状态
                    handle_task_ready(current_print_name);
                    // 这里不再修改状态，完全由 handle_task_ready() 控制
                }
            }
            break;
        case STARTMARK:

            //autotest：直接跳转到finished
            if(AUTO_TEST)
            {
                QTimer::singleShot(5, this, [this]() {
                    slot_mark_FinishStart();
                });
             }
             else
             {
                 handle_task_startMark();
             }

            break;
        case CHANGESLOT:
            handle_task_changeSlot();
            break;
        case STOPWAIT:
            handle_task_stopWait();
            break;
        case FINISHED:
            handle_task_finished();
            break;
        case ENDED:
            handle_task_ended();
            break;
        case EXCEPTION:
            handle_task_exception();
            break;
        }

        //autotest
        if(AUTO_TEST)
        {
            QThread::msleep(1000);
        }
        else
        {
            QThread::msleep(100);
        }

    }
}
*/

void MainWindow::slotOnMcuVersion(QString result)
{
    m_mcuVersion = result;
}

void MainWindow::slotOnEndPrintState(QString result, int stateValue)
{
    m_endPrintState = stateValue;
    qDebug() << "EndPrintState:" << result << "stateValue:" << stateValue;
}

void MainWindow::slot_setfanoff()
{
        if(!m_FanOff_delay_count)
            return;
        //关掉风扇
        if(m_FanOff_delay_count > 0)
        {
            m_FanOff_delay_count --;

            //Logger::instance().log("正在倒计时关闭风扇,m_FanOff_delay_count===", QString::number(m_FanOff_delay_count));
            qDebug() << "正在倒计时关闭风扇当前时间戳"  << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
            if(m_FanOff_delay_count < 1)
            {

                //Logger::instance().log("正在倒计时关闭风扇,m_FanOn_Start===", QString::number(m_FanOn_Start));
                if(m_FanOn_Start)
                {
                   m_FanOn_Start = false;
                   m_pMarkHandle->setFanOff();
                   //Logger::instance().log("触发setFanOff()===", QString::number(m_FanOn_Start));
                }
            }
        }
}

void MainWindow::on_print_finished(QString print_name)
{
    QString task_name;
    print_list_panel->setPrintTaskFinished(print_name, task_name);
    task_list_panel->increaseTaskSta(task_name);

    QStringList task_text_split = current_print_task.split(SPLIT_SYMBOL);
    // 检查task_text_split是否为空且包含足够的元素，避免索引越界
    if (task_text_split.isEmpty() || task_text_split.size() <= TaskTextPos::TASK_TYPE) {
        qDebug() << "Invalid task_text_split format:" << current_print_task;
        m_printStateMachine->setState(PrintProcessSta::FINISHED);
        qDebug() << "Print finished:" << print_name;
        return;
    }

    int task_type = task_text_split[TaskTextPos::TASK_TYPE].toUShort();

    try
    {
        //插入打印数据表
        print_task_manager.insertPrintTask(print_name,current_print_task,task_type,"");
    }
    catch (const std::exception &e)
    {
//        print_task_manager.insertPrintTask(print_name,current_print_task,task_type,"");
        Logger::instance().log("Exception in insertPrintTask: ", e.what());
    }
    catch (...)
    {
        Logger::instance().log("Unknown exception in insertPrintTask","");

    }


    m_printStateMachine->setState(PrintProcessSta::FINISHED);
    qDebug() << "Print finished:" << print_name;
}

void MainWindow::slot_mark_State(int stateNum, QString desc)
{
   qDebug() << QString("stateNum = %1,desc = %2").arg(stateNum).arg(desc);

//   auto result = getStatusMessage(stateNum);
//   QString message = result.first;
//   int soundLevel = result.second;

   McuAlertInfo info = getStatusMessage(stateNum);
   QString message = info.displayText;
   int soundLevel = info.soundLevel;
   QString code = info.errorCode;
   if(!code.isEmpty())
   {
       message = code + ": " + message;
   }

   //初始化打印线程状态
   switch (stateNum)
   {
       case 0 :
         m_printStateMachine->setState(PrintProcessSta::READY);
         qDebug () << "0:初始化完成，标刻模块准备就绪";
         m_Init_finished = true;
         title_bar->setMachineSta(ready);
         m_turn_motor_state = 0;
         break;
       case 1 :
         //开启风扇
         break;
       case 4:
//         m_pMarkHandle->setFanOn(settings.create_setting.gas_treatment_speed_level_cbb);
//         m_FanOn_Start = true;
         //延时设置时间关闭
         setFanOffDelay(settings.create_setting.gas_treatment_auto_sleep_cbb);
         break;
       case 8 :
          //托盘电机正在运行
          qDebug () << "8:托盘电机正在运行";
          m_turn_motor_state = 1;
          break;
       case 9 :
          //托盘电机运行完成
          qDebug () << "9:托盘电机运行完成";
          m_turn_motor_state = 0;
          break;
       case -10:
         // 组织盒更换槽盒
         #ifdef USE_CASSETTE
            if(list_auto_btn->isChecked())
              m_printStateMachine->setState(PrintProcessSta::CHANGESLOT);
            qDebug () << "-10:当前选中的槽盒已用完";
         // 载玻片不更换直接异常处理
         #elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
            m_printStateMachine->setState(PrintProcessSta::EXCEPTION);
            Logger::instance().log("slot_mark_State1", desc);
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(message);
            if(soundLevel < 2)
            {
               title_bar->setMachineSta(error);
            }
            msg_box->Exec(soundLevel,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle);
            title_bar->setMachineSta(ready);
         #endif

         break;
   }

   if(stateNum < 0)
   {

       m_turn_motor_state = 0;
       if(-10!=stateNum)
       {
           qDebug () << "print_process_sta == PrintProcessSta::EXCEPTION";
           //print_process_sta = PrintProcessSta::EXCEPTION;
           //直接调用异常处理handle_task_exception() 不需要传递EXCPTION状态中断异常
           handle_task_exception();
           Logger::instance().log("slot_mark_State1", desc);
           DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
           msg_box->setContent(message);
           if(soundLevel < 2)
           {
              title_bar->setMachineSta(error);
           }

//           if(stateNum == -21)
//           {
//               //设置结束打印状态，目的为标刻遇到堵转，打印暂停不报警提示问题
//               m_endPrintState = 2;
//           }

           //           if((-11 != stateNum) && (-21 != stateNum))
           //准备耗材超时不弹窗
           if(-11 != stateNum)
           {
              if(stateNum != -1)
              {
                  if(msg_box->Exec(soundLevel,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle) == QDialog::Accepted)
                  {
                      //确认错误后，Title状态信息改为READY
                      //title_bar->setMachineSta(ready);
                  }
              }
              else
              {
                   msg_box->exec();
                   //title_bar->setMachineSta(ready);
              }
           }
       }
       else
       {
           //组织盒更换槽盒过程，槽盒已用完，报警-10处理，备选槽盒都遍历完，弹窗提示
           #ifdef USE_CASSETTE
           if(m_current_slots.size() < 1)
           {

               handle_task_exception();
               Logger::instance().log("slot_mark_State2", desc);

               stopMark();
               DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
               msg_box->setContent(message);
               if(soundLevel < 2)
               {
                  title_bar->setMachineSta(error);
               }

               //bool is_alarm_sound = settings.create_setting.alarm_sound_onoff_btn;
               //以此判断是否打开声音
               msg_box->Exec(soundLevel,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle);

               title_bar->setMachineSta(ready);
           }
           #endif
       }
   }
   status_bar->setStatetext(message);
}

void MainWindow::slot_mark_FinishStart()
{
    qDebug() << "Response signalMarkFinished()";

    on_print_finished(current_print_name);
}

void MainWindow::slot_mark_AlreadyStopped()
{
    //READ状态由标刻模块给出
    qDebug() << "Response signalAlreadyStopped()";

    if(false == list_auto_btn->isChecked())
    {
        print_list_panel->setPrintTaskState(current_print_name, PrintSta::PRINT_STA_WAIT);

        if(edit_panel->GetAlternatingPrint() && m_pMarkHandle->getPreparedBoxIndex() != 0xFF)
        {
            qDebug() << "slot_mark_AlreadyStopped: 还有提前准备好的槽盒，不切换状态";
        }
        else
        {
            m_printStateMachine->setState(PrintProcessSta::READY);
        }
    }
}

void MainWindow::slot_on_prepared_box_ready(quint8 boxIndex)
{
    qDebug() << "交替qDebug跟踪-slot_on_prepared_box_ready: 槽盒" << boxIndex << "准备就绪";

    QMutexLocker locker(&m_mutex);

    QString firstTask = print_list_panel->getFirstWaitingTaskNameFromModel();
    if (firstTask.isEmpty())
    {
        qDebug() << "交替qDebug跟踪-slot_on_prepared_box_ready: 没有等待的任务";
        return;
    }

    qDebug() << "交替qDebug跟踪-slot_on_prepared_box_ready: 获取到任务" << firstTask;

    QSettings task_list_ini(PRINT_LIST_INI, QSettings::IniFormat);
    task_list_ini.beginGroup("TaskList");
    QString task_str = task_list_ini.value(firstTask, "").toString();
    task_list_ini.endGroup();

    if (task_str.isEmpty())
    {
        qDebug() << "交替qDebug跟踪-slot_on_prepared_box_ready: 未找到任务数据";
        return;
    }

    current_print_name = firstTask;
    current_print_task = task_str;
    // 设置当前任务到TaskScheduler，避免重复调度
    m_taskScheduler->setCurrentTask(current_print_name);

    QStringList task_text_split = task_str.split(SPLIT_SYMBOL);

    QStringList task_text_list;
    task_text_list = task_text_split.mid(TaskTextPos::TASK_TEXT, MAX_INPUT_PANEL);

    QString task_temp_name = task_text_split[TaskTextPos::TASK_TEMP];
    QString task_name = task_text_split[TaskTextPos::TASK_NAME];
    unsigned task_type = task_text_split[TaskTextPos::TASK_TYPE].toUShort();

    QImage task_image;
    if(task_type==TaskType::REMOTE_IMAGE_TASK)
    {
        if (task_list_panel) {
            QString datePart = task_name.mid(5, 8);
            QString filePath = task_list_panel->getTaskdataDir() + datePart + "/";
            QString fullFileName = filePath + task_name + ".png";
            task_image.load(fullFileName);
        } else {
            qDebug() << "ERROR: task_list_panel is null";
            return;
        }
    }

    TemplateAttr task_temp_Attr;
    if(!read_taskTemplate_param(task_temp_name, task_temp_Attr))
    {
        if (edit_panel)
        {
            task_temp_name = edit_panel->GetCurrentTemplateName();
            read_taskTemplate_param(task_temp_name,task_temp_Attr);
        }
    }

    for (int i=0;i<MAX_INPUT_PANEL-1;i++)
    {
        if(task_temp_Attr.panel_attr[i].sta.compare("hidden")==0)
        {
            task_text_list[i] = "";
        }

        if(task_temp_Attr.panel_attr[i].sta.compare("fixed")==0)
        {
            int max_width = task_temp_Attr.panel_attr[i].min_width;
            task_text_list[i] = task_temp_Attr.panel_attr[i].input_text.left(max_width);
        }
    }

    task_text_list[MAX_INPUT_PANEL-1] = "";
    if(task_temp_Attr.panel_attr[MAX_INPUT_PANEL-1].sta.compare("display")==0)
    {
        QString dt_format = task_temp_Attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
        QString task_datetime = QDateTime::currentDateTime().toString(dt_format);
        task_text_list[MAX_INPUT_PANEL-1] = task_datetime;
    }

    QString text_index = task_temp_Attr.code_attr.text_index;
    QString time_alias_index = QString("%1").arg(TIME_ALIAS_INDEX);
    if(text_index.contains(time_alias_index))
    {
        QString dt_format = task_temp_Attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
        QString task_datetime = QDateTime::currentDateTime().toString(dt_format);
        task_text_list[MAX_INPUT_PANEL-1] = task_datetime;
    }

    QList<QLabel*> list;
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(!task_text_list[i].isEmpty())
        {
            QString field_text;
            int max_width = task_temp_Attr.panel_attr[i].min_width;
            field_text = task_text_list[i].left(max_width);
            task_text_list[i] = field_text;

            if(task_temp_Attr.panel_attr[i].sta.compare("invisible")==0)
            {
                field_text = "";
            }

            QLabel *pLbl = new QLabel(field_text);

            pLbl->move(task_temp_Attr.panel_attr[i].pos_x, task_temp_Attr.panel_attr[i].pos_y);
            pLbl->setFont(QFont(task_temp_Attr.panel_attr[i].font, task_temp_Attr.panel_attr[i].font_size, QFont::Normal));

            QString tempText = task_temp_Attr.panel_attr[i].input_text.left(max_width);
            QString textAlign = settings.create_setting.text_align_cbb;
            setLabelAlignmentInfo(pLbl,tempText,textAlign,task_temp_Attr.panel_attr[i].pos_x);

            if(i == MAX_INPUT_PANEL-1)
            {
                if(task_temp_Attr.panel_attr[i].sta.compare("invisible")==0)
                {
                    pLbl->move(-9999,-9999);
                }
            }
            list << pLbl;
        }
    }

    if(task_temp_Attr.logo_attr.is_display)
    {
        QLabel *plogoLbl = new QLabel();
        plogoLbl->move(task_temp_Attr.logo_attr.pos_x, task_temp_Attr.logo_attr.pos_y);
        plogoLbl->setFixedWidth(task_temp_Attr.logo_attr.width);
        plogoLbl->setFixedHeight(task_temp_Attr.logo_attr.height);
        QString temp_logo_file = QString("%1/%2/%3")
                .arg(USER_TEMPLATES_DIR)
                .arg(task_temp_Attr.name)
                .arg(task_temp_Attr.logo_attr.file_name);
        plogoLbl->setObjectName(temp_logo_file);
        list << plogoLbl;
    }

    QStringList bar_code_list;
    filterTaskText(task_text_list, text_index, bar_code_list);

    BarcodeInfo bcInfo = edit_panel->makeMarkBarCodeInfo(task_temp_Attr,bar_code_list);
    if(bar_code_list.isEmpty())
    {
        bcInfo.bcSize.setHeight(-1);
        bcInfo.bcSize.setWidth(-1);
    }

    int rotate;
    switch(task_temp_Attr.code_attr.direct_val)
    {
    case Direct::Up:
        rotate = 180;
        break;
    case Direct::Down:
        rotate = 0;
        break;
    case Direct::Left:
        rotate = 90;
        break;
    case Direct::Right:
        rotate = 270;
        break;
    default:
        rotate = 0;
    }

    QString textAlign = settings.create_setting.text_align_cbb;
    Align_type align = convertAlignType(textAlign);

    m_slots_current_index = boxIndex;
    m_pMarkHandle->setPreparedBoxIndex(0xFF);

    print_list_panel->setPrintTaskState(current_print_name, PrintSta::PRINT_STA_PRINTING);
    m_printStateMachine->setState(PrintProcessSta::STARTMARK);

    m_pMarkHandle->startMarkPicBuildIn(quint8(m_slots_current_index),list,bcInfo,MarkHandle::Rotate(rotate),false,align);

    qDebug() << "交替qDebug跟踪-slot_on_prepared_box_ready: 开始标刻槽盒" << boxIndex;
}

QString MainWindow::convertRemoteTaskData(const QString &input)
{
    // 空输入检查
    if (input.isEmpty()) {
        qDebug() << "Warning: convertRemoteTaskData received empty input";
        return input;
    }

#if MAX_INPUT_PANEL > 8
    try {
        // 先用 "../.." 拆分
        QStringList parts = input.split("../..", Qt::KeepEmptyParts);

        // 如果至少有9个以上元素
        if (parts.size() > 8) {
            // 第8段数据（下标7）
            QString rawField = parts[7];

            // 用 SUB_SPLIT_SYMBOL 拆分
            QStringList subFields = rawField.split(SUB_SPLIT_SYMBOL, Qt::KeepEmptyParts);

            // 准备9个定长字段,最后一个空段为模版字段预留
            QStringList fixedFields;
            for (int i = 0; i < 8; ++i) {
                if (i < subFields.size())
                    fixedFields << subFields[i];
                else
                    fixedFields << ""; // 不足填空
            }
            //第9个为模版预留空字段
            fixedFields << "";

            // 替换第8段为插入的 9 个字段
            parts.removeAt(7);
            for (int i = 0; i < 9; ++i) {
                parts.insert(7 + i, fixedFields[i]);
            }
        }

        // 重新拼接
        return parts.join("../..");
    } catch (const std::exception& e) {
        // 捕获任何异常，防止程序崩溃
        qDebug() << "Error in convertRemoteTaskData: " << e.what();
        Logger::instance().log("Error in convertRemoteTaskData:", e.what());
        Logger::instance().log("Error in convertRemoteTaskData.input===>>>>", input);

        return input; // 返回原始输入作为后备
    }
#else
    // 如果 MAX_INPUT_PANEL <= 8，不需要改动
    return input;
#endif
}

Align_type MainWindow::convertAlignType(QString textalign)
{
    Align_type ret = Left_0;
    if(textalign.compare("left")==0)
    {
        ret = Left_0;
    }
    if(textalign.compare("middle")==0)
    {
        ret = Center_1;
    }
    if(textalign.compare("right")==0)
    {
        ret = Right_2;
    }

    return ret;
}

void MainWindow::slot_RecvTextTask(quint64 taskIndex, QStringList boxIndex, QByteArray text)
{
    try
    {
    // 输入参数有效性检查
        if (text.isEmpty()) {
            qDebug() << "Error: Received empty text data";
            return;
        }

        // 指针有效性检查
        if (!task_list_panel) {
            qDebug() << "Error: task_list_panel is null";
            return;
        }


        qDebug() << "taskIndex=" << QString::number(taskIndex);
        qDebug() << "boxIndex=" << boxIndex;
        qDebug() << "text=" << text;

        // 任务数量检查
        int row_cnt = task_list_panel->getListCount();
        if(1+row_cnt > MAX_TASKS)
        {
            qDebug() << "Task list is full, cannot add new task";
            return;
        }

        QStringList taskTextList;
        // 安全地将 QByteArray 转换为 QString
        QString textStr = QString::fromUtf8(text);
        qDebug() << "converted text=" << textStr;

        // 调用转换函数并检查结果
        QString newRemoteText = convertRemoteTaskData(textStr);
        if (newRemoteText.isEmpty() && !textStr.isEmpty())
        {
            qDebug() << "Warning: convertRemoteTaskData returned empty string";
            // 使用原始文本作为后备
            newRemoteText = textStr;
        }

        qDebug() << "newRemoteText=" << newRemoteText;
        taskTextList.append(newRemoteText);

        // 安全地添加任务
        QStringList add_to_print_list = task_list_panel->addTextListToTaskTable(taskTextList,TaskType::REMOTE_TEXT_TASK);

        //远程任务为单个接收，到达最大条数只弹一次提示
        if(1+row_cnt == MAX_TASKS)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(QString("%1%2").arg(tr("已到达任务列表最大任务数")).arg(MAX_TASKS));
            msg_box->exec();
        }


        bool is_auto_print = settings.print_setting.auto_print_after_task_established_btn;
        //添加到打印列表
        if(is_auto_print && print_list_panel && list_tab_widget) // 添加指针有效性检查
        {
            int row_cnt = print_list_panel->getListCount();
            if(1+row_cnt > MAX_TASKS)
            {
                return;
            }

//            if(list_tab_widget->currentIndex() < 1)
//            {
//                list_tab_widget->setCurrentIndex(1);
//            }

            //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList());
            print_list_panel->addTaskListToPrintTable(add_to_print_list,false,false);

            if(1+row_cnt == MAX_TASKS)
            {
                DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
                msg_box->setContent(QString("%1%2").arg(tr("已到达打印列表最大任务数")).arg(MAX_TASKS));
                msg_box->exec();
            }
        }
    }
    catch (const std::exception& e)
    {
        Logger::instance().log("Exception in slot_RecvTextTask()",QString("Exception====%1").arg(e.what()));
        Logger::instance().log("Error text in slot_RecvTextTask()",QString("Error text====%1") + text);
        //       Logger::instance().log("Error in slot_RecvTextTask2.textStr===>>>>", textStr);
        //Logger::instance().log("Error in slot_RecvTextTask2.newRemoteText===>>>>", newRemoteText);
    }

    catch (...)
    {
        Logger::instance().log("Unknown exception in slot_RecvTextTask","");
    }
}

void MainWindow::slot_RecvPicTask(quint64 taskIndex, QStringList boxIndex, QByteArray pic)
{

    qDebug() << "taskIndex=" << QString::number(taskIndex);
    qDebug() << "boxIndex=" << boxIndex;

    int row_cnt = task_list_panel->getListCount();
    if(1+row_cnt > MAX_TASKS)
    {
        return;
    }

    QString imageName;


    QStringList add_to_print_list = task_list_panel->addImageTaskToTaskTable(imageName,pic,false,false);
    //task_list_panel->addImageTaskToTaskTable(imageName,pic,false,true);

    //远程任务为单个接收，到达最大条数只弹一次提示
    if(1+row_cnt == MAX_TASKS)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(QString("%1%2").arg(tr("已到达任务列表最大任务数")).arg(MAX_TASKS));
        msg_box->exec();
    }

    bool is_auto_print = settings.print_setting.auto_print_after_task_established_btn;
    //添加到打印列表
    if(is_auto_print)
    {
        int row_cnt = print_list_panel->getListCount();
        if(1+row_cnt > MAX_TASKS)
        {
            return;
        }

//        if(list_tab_widget->currentIndex() < 1)
//        {
//            list_tab_widget->setCurrentIndex(1);
//        }
        //print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList(),false,false);
        if(add_to_print_list.size() > 0)
        {
           print_list_panel->addTaskListToPrintTable(add_to_print_list,false,false);
        }

        if(1+row_cnt == MAX_TASKS)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(QString("%1%2").arg(tr("已到达打印列表最大任务数")).arg(MAX_TASKS));
            msg_box->exec();
        }
    }
}

void MainWindow::slot_RecvRemoteState(int stateNum, QString desc)
{
    qDebug() << QString("stateNum = %1,desc = %2").arg(stateNum).arg(desc);
    status_bar->setStatetext2(desc);
}

void MainWindow::slot_RecvRemoteLinkState(bool isOnline)
{
    bool remoteOnOff = settings.remote_text_setting.remote_text_print_btn;

    //if(remoteOnOff)
        status_bar->setRemotePrintLinkSta(isOnline);
}

void MainWindow::slot_RecvRemoteComLinkState(bool isOnline)
{
    status_bar->setRemotePrintLinkSta(isOnline);
}

void MainWindow::slot_RecvRemoteSyncLinkState(bool isOnline)
{
    status_bar->setSyncPrintLinkSta(isOnline);
}

void MainWindow::slot_McuState(QString msg)
{
    //加载前清空
    m_nMcuStateDataList = JsonDataList();
    m_nMcuStateDataList = JsonDataList::parseJson(msg);

    for (const auto& item : m_nMcuStateDataList.getDataList())
    {
        qDebug() << "Key: " << item.key << ", Value: " << item.value;

        QStringList mcuStateNotifyKeyList ;
        mcuStateNotifyKeyList << "显示面板已合上" << "卸载滑轨已接好" << "载玻片物料盒是否已合上" ; //<<"组织盒(载玻片)是否已经堆满"
        if(!mcuStateNotifyKeyList.contains(item.key))
        {
            continue;
        }
        bool ok;
        double value = item.value.toDouble(&ok);
        QString targetKey = item.key;
//        if((print_list_panel->getPrintActionCheckedState()) && ok && value > 0)
        if(list_auto_btn->isChecked() && ok && value > 0)
        {
//            auto result = m_nMcuStateDataList.getMcuAliasByKey(targetKey);
//            QString alias = result.first;
//            int level = result.second;

            McuAlertInfo info = m_nMcuStateDataList.getMcuAlertInfoByKey(item.key);
            QString alias = info.displayText;
            int level = info.soundLevel;
            QString code = info.errorCode;
            if(!code.isEmpty())
            {
                alias = code + ": " + alias;
            }

            handle_task_exception();
            //槽盒翻转座合闭
            stopMark();
            Logger::instance().log("slot_McuState", alias);
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(alias);
            if(level < 2)
            {
               title_bar->setMachineSta(error);
            }
            msg_box->Exec(level,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle);
            title_bar->setMachineSta(ready);
            return;
        }
    }
}

void MainWindow::slot_McuAlarm(QString msg)
{
    //加载前清空
    m_nMcuAlarmDataList = JsonDataList();
    m_nMcuAlarmDataList = JsonDataList::parseJson(msg);

    QStringList mcuAlarmNotifyKeyList ;
    mcuAlarmNotifyKeyList << "组织盒(载玻片)是否已经堆满" << "翻转电机(收纳盒)电机错误" << "卸载耗材错误";
    for (const auto& item : m_nMcuAlarmDataList.getDataList())
    {
        bool ok;
        double value = item.value.toDouble(&ok);

        if(mcuAlarmNotifyKeyList.contains(item.key))
        {
            qDebug() << "slot_McuAlarm(===)组织盒(载玻片)是否已经堆满.Key: " << item.key << ", Value: " << item.value;
        }

//        if (((print_list_panel->getPrintActionCheckedState()) || (m_endPrintState > 0)) && ok && value > 0)
        if (((list_auto_btn->isChecked()) || (m_endPrintState > 0)) && ok && value > 0)
        {
//            auto result = m_nMcuAlarmDataList.getMcuAliasByKey(item.key);
//            QString alias = result.first;
//            int level = result.second;

            m_endPrintState = -1;

            McuAlertInfo info = m_nMcuStateDataList.getMcuAlertInfoByKey(item.key);
            QString alias = info.displayText;
            int level = info.soundLevel;
            QString code = info.errorCode;
            if(!code.isEmpty())
            {
                alias = code + ": " + alias;
            }

            handle_task_exception();
            //槽盒翻转座合闭
            stopMark();
            Logger::instance().log("slot_McuAlarm", alias);
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(alias);

            if(level < 2)
            {
               title_bar->setMachineSta(error);
            }
            msg_box->Exec(level,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle);
            title_bar->setMachineSta(ready);

            status_bar->setStatetext2(alias);
            return;
        }
    }
}

void filterTaskText(const QStringList &task_text_list, const QString &text_index, QStringList &bar_code_list) {
    bar_code_list.clear(); // 清空 bar_code_list

    // 通过 ';' 分割 text_index，得到序号列表
    QStringList indices = text_index.split(';', QString::SkipEmptyParts);

    // 遍历 indices 并挑选 task_text_list 中对应的元素
    for (const QString &index : indices) {
        bool ok;
        int idx = index.toInt(&ok); // 将字符串转换为整数
        if (ok && idx >= 0 && idx < task_text_list.size()) { // 检查序号是否有效
            const QString &text = task_text_list[idx];
            bar_code_list.append(text);
//            if (!text.isEmpty()) { // 检查字符串是否为空
//                bar_code_list.append(text);
//            }
        }
    }
}

QString MainWindow::make_remote_text_data(const QString &task_text)
{
     /*
    QStringList field_text_list = task_text.split(SPLIT_SYMBOL).mid(0, MAX_INPUT_PANEL + 2);
    QString remote_data = field_text_list.join(SPLIT_SYMBOL);

    QString text_front_symbol = settings.remote_text_setting.print_front_symbol_le;
    QString text_seprate_symbol = settings.remote_text_setting.print_delimiter_le;
    QString text_tail_symbol = settings.remote_text_setting.print_tail_symbol_le;
    QString slots_front_symbol = settings.remote_text_setting.slots_front_symbol_le;
    QString slots_seprate_symbol = settings.remote_text_setting.slots_delimiter_le;
    QString slots_tail_symbol = settings.remote_text_setting.slots_tail_symbol_le;

    QString remote_data, text_data, slots_data;

    QStringList text_list = task_text.split(SPLIT_SYMBOL).mid(TaskTextPos::TASK_TEXT, MAX_INPUT_PANEL-1);
    text_data = text_list.join(text_seprate_symbol);
    text_data.prepend(text_front_symbol);
    text_data.append(text_tail_symbol);

    qDebug() << "text_data:" << text_data;

    QStringList slot_list = task_text.split(SPLIT_SYMBOL)[TaskTextPos::TASK_SLOTS].split(",");
    slots_data = slot_list.join(slots_seprate_symbol);
    slots_data.prepend(slots_front_symbol);
    slots_data.append(slots_tail_symbol);


    qDebug() << "slots_data:" << slots_data;

    remote_data.append(text_data);
    remote_data.append(slots_data);

    return remote_data;
   */

    //只取内容+temp+hopper
#if 0
    QStringList field_text_list = task_text.split(SPLIT_SYMBOL).mid(0, MAX_INPUT_PANEL + 2);
    QString remote_data = field_text_list.join(SPLIT_SYMBOL);
#else
    //兼容旧版本，扩展字段用子间隔符放到第8个字段
    QStringList task_text_split_list = task_text.split(SPLIT_SYMBOL);
    QStringList field_text_list, ext_field_text_list;
    QStringList remote_data_list;

    for (int i = 0; i < 10; i++)
    {// 协议字段初始化：8个字段+Template+hopper=10
        remote_data_list.append("");
    }

    if(MAX_INPUT_PANEL > 7)
    {
        field_text_list = task_text_split_list.mid(0, 7);
        ext_field_text_list = task_text_split_list.mid(7, MAX_INPUT_PANEL - 7);
    }
    else
    {
        field_text_list = task_text_split_list.mid(0, MAX_INPUT_PANEL);
    }

    for (int i = 0; i < 7 && i < field_text_list.size(); i++)
    {// 填写前7个字段
        remote_data_list.replace(i, field_text_list.at(i));
    }
    // 填写8、9、10字段
    remote_data_list.replace(7, ext_field_text_list.join(SUB_SPLIT_SYMBOL));
    remote_data_list.replace(8, task_text_split_list[TaskTextPos::TASK_TEMP]);
    remote_data_list.replace(9, task_text_split_list[TaskTextPos::TASK_SLOTS]);

    QString remote_data = remote_data_list.join(SPLIT_SYMBOL);

#endif

    return remote_data;
}

void MainWindow::startMark()
{
    try {
        QStringList task_text_split;
        QStringList task_text_list;

        //解析打印任务
        task_text_split = current_print_task.split(SPLIT_SYMBOL);

        // 检查task_text_split是否包含足够的元素，防止索引越界
        if (task_text_split.size() <= TaskTextPos::TASK_TYPE)
        {
            qDebug() << "ERROR: Invalid task format, insufficient elements in task_text_split";
            Logger::instance().log("startMark()", "ERROR: Invalid task format, insufficient elements in task_text_split");
            return;
        }

        task_text_list = task_text_split.mid(TaskTextPos::TASK_TEXT, MAX_INPUT_PANEL);

        //从当前打印任务获取所选用的模版参数
        QString task_temp_name = task_text_split[TaskTextPos::TASK_TEMP];
        QString task_name = task_text_split[TaskTextPos::TASK_NAME];
        unsigned task_type = task_text_split[TaskTextPos::TASK_TYPE].toUShort();

        //选择当前模版
        //task_temp_name = edit_panel->GetCurrentTemplateName();

        QImage task_image;
        //从本地目录里加载远程图像任务
        if(task_type==TaskType::REMOTE_IMAGE_TASK)
        {
            // 检查task_list_panel是否为空
            if (task_list_panel) {
                QString datePart = task_name.mid(5, 8);
                QString filePath = task_list_panel->getTaskdataDir() + datePart + "/";
                QString fullFileName = filePath + task_name + ".png";
                task_image.load(fullFileName);
            } else {
                qDebug() << "ERROR: task_list_panel is null";
                Logger::instance().log("startMark()", "ERROR: task_list_panel is null");
            }
        }
        //读取模版
        TemplateAttr  task_temp_Attr;
        //read_taskTemplate_param(task_temp_name,task_temp_Attr);

        //如果读不到任务里的自定义模板(包括固定模板)，则选择当前模板
        if(!read_taskTemplate_param(task_temp_name, task_temp_Attr))
        {
            //选择当前模版
            if (edit_panel)
            {
                task_temp_name = edit_panel->GetCurrentTemplateName();
                read_taskTemplate_param(task_temp_name,task_temp_Attr);
            }
            else
            {
                qDebug() << "ERROR: edit_panel is null";
                Logger::instance().log("startMark()", "ERROR: edit_panel is null");
                return;
            }
        }

        //加载固定标签
        for (int i=0;i<MAX_INPUT_PANEL-1;i++)
        {
            //跳过不可用标签，不加载
            if(task_temp_Attr.panel_attr[i].sta.compare("hidden")==0)
            {
                task_text_list[i] = "";
            }

            if(task_temp_Attr.panel_attr[i].sta.compare("fixed")==0)
            {
                int max_width = task_temp_Attr.panel_attr[i].min_width;
                task_text_list[i] = task_temp_Attr.panel_attr[i].input_text.left(max_width);
            }
        }

        //获取任务时间
        task_text_list[MAX_INPUT_PANEL-1] = "";
        if(task_temp_Attr.panel_attr[MAX_INPUT_PANEL-1].sta.compare("display")==0)
        {
            QString dt_format = task_temp_Attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
            QString task_datetime = QDateTime::currentDateTime().toString(dt_format);
            task_text_list[MAX_INPUT_PANEL-1] = task_datetime;
        }

        //模版勾选时间到二维码,但是不显示时间文本
        QString text_index = task_temp_Attr.code_attr.text_index;
        QString time_alias_index = QString("%1").arg(TIME_ALIAS_INDEX);
        if(text_index.contains(time_alias_index))
        {
            QString dt_format = task_temp_Attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
            QString task_datetime = QDateTime::currentDateTime().toString(dt_format);
            task_text_list[MAX_INPUT_PANEL-1] = task_datetime;
        }

        QList<QLabel*> list;
        for (int i=0; i<MAX_INPUT_PANEL; i++)
        {
            if(!task_text_list[i].isEmpty())
            {
                // 改为截取指定最大长度
                QString field_text;
                int max_width = task_temp_Attr.panel_attr[i].min_width;
                field_text = task_text_list[i].left(max_width);
                //更新 task_text_list每个截取最大长度后的子项
                task_text_list[i] = field_text;

                //对隐藏字段文本的预览要清空，但是要加入二维码里面
                if(task_temp_Attr.panel_attr[i].sta.compare("invisible")==0)
                {
                    field_text = "";
                }

                QLabel *pLbl = new QLabel(field_text);
                qDebug() << "max_width" << max_width;
                //             QLabel *pLbl = new QLabel(task_text_list[i]);

                pLbl->move(task_temp_Attr.panel_attr[i].pos_x, task_temp_Attr.panel_attr[i].pos_y);
                pLbl->setFont(QFont(task_temp_Attr.panel_attr[i].font, task_temp_Attr.panel_attr[i].font_size, QFont::Normal));

                //重新对齐标刻pLbl settings
                QString tempText = task_temp_Attr.panel_attr[i].input_text.left(max_width);
                QString textAlign = settings.create_setting.text_align_cbb;

                //realignLabelText(pLbl,tempText,textAlign,task_temp_Attr.panel_attr[i].pos_x);
                setLabelAlignmentInfo(pLbl,tempText,textAlign,task_temp_Attr.panel_attr[i].pos_x);

                //模版勾选时间到二维码,但是不显示时间文本
                if(i == MAX_INPUT_PANEL-1)
                {
                    //隐藏时间标签特殊处理，使标刻坐标超出范围
                    if(task_temp_Attr.panel_attr[i].sta.compare("invisible")==0)
                    {
                        pLbl->move(-9999,-9999);
                    }
                }
                list << pLbl;
            }
        }

        //传入的标刻LOGO
        if(task_temp_Attr.logo_attr.is_display)
        {
            //增加logo标刻
            QLabel *plogoLbl = new QLabel();
            plogoLbl->move(task_temp_Attr.logo_attr.pos_x, task_temp_Attr.logo_attr.pos_y);
            plogoLbl->setFixedWidth(task_temp_Attr.logo_attr.width);
            plogoLbl->setFixedHeight(task_temp_Attr.logo_attr.height);
            QString temp_logo_file = QString("%1/%2/%3")
                    .arg(USER_TEMPLATES_DIR)
                    .arg(task_temp_Attr.name)
                    .arg(task_temp_Attr.logo_attr.file_name);
            plogoLbl->setObjectName(temp_logo_file);
            list << plogoLbl;
        }

        //获取组成条形码的文本list
        QStringList bar_code_list;
        //QString text_index = task_temp_Attr.code_attr.text_index;
        filterTaskText(task_text_list, text_index, bar_code_list);

        //设置标刻等级参数，同时区分文字打印和图片打印
        int mark_level = edit_panel->GetSlotColorMarkLevel(m_slots_current_index+1);
        if(mark_level < 1 && mark_level >10)
        {
            mark_level = 8;
        }

        //转换标刻方向
        int rotate;
        switch(task_temp_Attr.code_attr.direct_val)
        {
        case Direct::Up:
            rotate = 180;
            break;
        case Direct::Down:
            rotate = 0;
            break;
        case Direct::Left:
            rotate = 90;
            break;
        case Direct::Right:
            rotate = 270;
            break;
        default:
            rotate = 0;
        }

        // 检查m_pMarkHandle是否为空
        if (!m_pMarkHandle) {
            qDebug() << "ERROR: m_pMarkHandle is null";
            // 清理创建的QLabel对象
            qDeleteAll(list.begin(), list.end());
            return;
        }

        if(task_type==TaskType::REMOTE_IMAGE_TASK)
        {
            m_pMarkHandle->setMarkPenArg(m_mark_image_level_args[mark_level-1]);
        }
        else
        {
            m_pMarkHandle->setMarkPenArg(m_mark_level_args[mark_level-1]);
        }

#ifdef USE_CASSETTE
        //生成标刻图片
        //    QImage image = edit_panel->createMarkPicture(list,task_temp_Attr,bar_code_list);
        //    QThread::msleep(100);
        //    qDebug() << QString("startMark(%1)").arg(quint8(m_slots_current_index));

        if(task_type==TaskType::REMOTE_IMAGE_TASK)
        {
            //m_pMarkHandle->startMark(quint8(m_slots_current_index) ,task_image);
            m_pMarkHandle->startMarkPicRemote(quint8(m_slots_current_index) ,task_image, QPoint(0, 0),QSize(PIXEL_VIEW_WIDTH, PIXEL_VIEW_HEIGHT),MarkHandle::Rotate(rotate));
        }
        else
        {
            //m_pMarkHandle->startMark(quint8(m_slots_current_index) , image);
            BarcodeInfo bcInfo = edit_panel->makeMarkBarCodeInfo(task_temp_Attr,bar_code_list);
            //如果填入都是空字符，二维码是空的情况
            if(bar_code_list.isEmpty())
            {
                bcInfo.bcSize.setHeight(-1);
                bcInfo.bcSize.setWidth(-1);
            }

            QString textAlign = settings.create_setting.text_align_cbb;
            Align_type align = convertAlignType(textAlign);
            m_pMarkHandle->startMarkPicBuildIn(quint8(m_slots_current_index),list,bcInfo,MarkHandle::Rotate(rotate),align);

            QString LoggerStr;
            QDebug debug(&LoggerStr);
            debug << "\n" << "bcInfo.bcPos==" << bcInfo.bcPos;
            debug << "\n" << "bcInfo.bcSize==" << bcInfo.bcSize;
            debug << "\n" << "bcInfo.bcType==" << bcInfo.bcType;
            debug << "\n" << "bcInfo.bcStrList==" << bcInfo.bcStrList;
            debug << "\n" << "bcInfo.bcStrSeparator==" << bcInfo.bcStrSeparator;
            debug << "\n" << "list==" << list;

            //Logger::instance().log("startMark(bcInfo,list)==========================", LoggerStr);
        }
        //qDeleteAll(list.begin(), list.end());
        //list.clear();

#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)

        BarcodeInfo bcInfo = edit_panel->makeMarkBarCodeInfo(task_temp_Attr,bar_code_list);
        if(task_type==TaskType::REMOTE_IMAGE_TASK)
        {
            m_pMarkHandle->startMarkPicRemote(quint8(m_slots_current_index) ,task_image,QPoint(0, 0), QSize(PIXEL_VIEW_WIDTH, PIXEL_VIEW_HEIGHT),MarkHandle::Rotate(rotate));
        }
        else
        {
            BarcodeInfo bcInfo = edit_panel->makeMarkBarCodeInfo(task_temp_Attr,bar_code_list);
            //如果填入都是空字符，二维码是空的情况
            if(bar_code_list.isEmpty())
            {
                bcInfo.bcSize.setHeight(-1);
                bcInfo.bcSize.setWidth(-1);
            }

            //判断是否交替打印
            bool alternatingPrint = edit_panel->GetAlternatingPrint();

            // 检查print_list_panel是否为空
            int print_row_cnt = 0;
            if (print_list_panel) {
                print_row_cnt = print_list_panel->getListCount();
            } else {
                qDebug() << "WARNING: print_list_panel is null, assuming single task";
                Logger::instance().log("startMark()", "WARNING: print_list_panel is null, assuming single task");
            }

            //最后一个打印任务不用交替
            if(print_row_cnt < 2)
            {
                alternatingPrint = false;
            }

            QString textAlign = settings.create_setting.text_align_cbb;
            Align_type align = convertAlignType(textAlign);

            m_pMarkHandle->startMarkPicBuildIn(quint8(m_slots_current_index),list,bcInfo,MarkHandle::Rotate(rotate),alternatingPrint,align);
        }

#endif

    }
    catch (const std::exception &e)
    {
        Logger::instance().log("Exception in addTextListToTaskTable: ", e.what());
    }
    catch (...)
    {
        Logger::instance().log("Unknown exception in addTextListToTaskTable","");
    }

}

void MainWindow::stopMark()
{
    QMutexLocker locker(&m_mutex);

    bool alternatingPrint = edit_panel->GetAlternatingPrint();
    int print_row_cnt = print_list_panel->getListCount();

    //stopMark the prev one when leftover the only one
//    if(print_row_cnt < 2)
//    {
//        m_pMarkHandle->stopMark((m_slots_current_index + 1) % 2);
//    }

    //m_pMarkHandle->stopMark(m_slots_current_index);
}

void MainWindow::on_task_list_btnMaxMin_clicked(bool isChecked)
{
    if(task_list_panel->isPanelMax() && print_list_panel->isPanelMax())
    {
        task_list_panel->setPanelMin();
        print_list_panel->setPanelMax();
    }
    else if(!task_list_panel->isPanelMax() && print_list_panel->isPanelMax())
    {
        task_list_panel->setPanelMax();
        print_list_panel->setPanelMax();
    }
    else if(task_list_panel->isPanelMax() && !print_list_panel->isPanelMax())
    {
        task_list_panel->setPanelMax();
        print_list_panel->setPanelMax();
    }
}

void MainWindow::on_print_list_btnMaxMin_clicked(bool isChecked)
{
    if(task_list_panel->isPanelMax() && print_list_panel->isPanelMax())
    {
        print_list_panel->setPanelMin();
        task_list_panel->setPanelMax();
    }
    else if(!task_list_panel->isPanelMax() && print_list_panel->isPanelMax())
    {
        print_list_panel->setPanelMax();
        task_list_panel->setPanelMax();
    }
    else if(task_list_panel->isPanelMax() && !print_list_panel->isPanelMax())
    {
        print_list_panel->setPanelMax();
        task_list_panel->setPanelMax();
    }
}

void MainWindow::on_get_task_list_info(const QString  & infoStr)
{
   qDebug() << "on_get_task_list_info1.taskinfo == " << infoStr;
   QStringList task_text_split;
   //解析打印任务
   task_text_split = infoStr.split(SPLIT_SYMBOL);

   QString taskName = task_text_split[TaskTextPos::TASK_NAME];
   QString taskType = task_text_split[TaskTextPos::TASK_TYPE];

   //从当前打印任务获取所选用的模版参数
   QString task_temp_name = task_text_split[TaskTextPos::TASK_TEMP];
   TemplateAttr selected_task_temp_attr;
   if(!read_taskTemplate_param(task_temp_name, selected_task_temp_attr))
   {
       task_temp_name = edit_panel->GetCurrentTemplateName();
       read_taskTemplate_param(task_temp_name,selected_task_temp_attr);
   }
   edit_panel->setCurrentTempAttr(selected_task_temp_attr);

   if(taskType.toInt() == TaskType::REMOTE_IMAGE_TASK)
   {
       // 文件路径

       QString datePart = taskName.mid(5, 8); // 从第 5 个字符开始提取 8 个字符
       QString filePath = task_list_panel->getTaskdataDir() + datePart + "/";
       QString fullFileName = filePath + taskName + ".png";
       qDebug() << "on_get_task_list_info.fullFileName===" << fullFileName;

       QImage image;
       image.load(fullFileName);
       edit_panel->load_image_to_markPreview(image);
   }
   else
   {
       //从当前打印任务获取所选用的模版参数
       //QString task_temp_name = task_text_split[TaskTextPos::TASK_TEMP];

//       TemplateAttr selected_task_temp_attr;
//       if(!read_taskTemplate_param(task_temp_name, selected_task_temp_attr))
//       {
//           task_temp_name = edit_panel->GetCurrentTemplateName();
//           read_taskTemplate_param(task_temp_name,selected_task_temp_attr);
//       }
//       edit_panel->setCurrentTempAttr(selected_task_temp_attr);

       edit_panel->load_text_to_markPreview(task_text_split);
       edit_panel->set_logo_to_markPreview(selected_task_temp_attr);
       edit_panel->clearInputPanelsFocus();
   }

   qDebug() << "on_get_task_list_info3.taskinfo == " << infoStr;
}

void MainWindow::on_application_about_to_quit()
{
    //退出线程
    if(watcher.isRunning())
    {
        future.cancel();
    }

    on_print_task_exit_request();
}

//void MainWindow::on_addTextListToTaskTable(const QStringList &textList, TaskType taskType)
//{
//    if(task_list_panel)
//    {
//        task_list_panel->addTextListToTaskTable(textList, TaskType::LOCAL_TASK);
//    }
//}

//void MainWindow::addTextListToTaskTable_OnThread(const QStringList &textList, TaskType taskType)
//{
//    //线程加载数据
//    QThread* thread = new QThread();
//    connect(thread, &QThread::started, thread, [=]()mutable{
//        qDebug()<<"started!";
//        thread->quit();
//    }, Qt::DirectConnection);
//    connect(thread, &QThread::finished, this, [=]()mutable{
//        thread->wait();
//        thread->deleteLater();
//        qDebug()<<"finished!";
//        on_addTextListToTaskTable(textList,taskType);
//    });
//    thread->start();
//}

//void MainWindow::on_addTaskListToPrintTable(const QStringList &taskList, bool isFront)
//{
//    if(task_list_panel && print_list_panel)
//    {
//        print_list_panel->addTaskListToPrintTable(task_list_panel->getSelectedTaskList(), isFront);
//    }
//}

//void MainWindow::addTaskListToPrintTable_OnThread(const QStringList &taskList, bool isFront)
//{
//    //线程加载数据
//    QThread* thread = new QThread();
//    connect(thread, &QThread::started, thread, [=]()mutable{
//        qDebug()<<"started!";
//        thread->quit();
//    }, Qt::DirectConnection);
//    connect(thread, &QThread::finished, this, [=]()mutable{
//        thread->wait();
//        thread->deleteLater();
//        qDebug()<<"finished!";
//        on_addTaskListToPrintTable(taskList,isFront);
//    });
//    thread->start();
//}

void MainWindow::setup_expired_tasks_monitor()
{
    // 初始化上次删除日期为当前日期
    last_expired_tasks_delete_date = QDate::currentDate();

    // 先执行一次
    task_list_panel->deleteExpiredTasks(settings.create_setting.auto_del_expired_tasks_cbb, last_expired_tasks_delete_date);

    expired_tasks_monitor_timer = new QTimer(this);

    connect(expired_tasks_monitor_timer, &QTimer::timeout, this, [=]
    {
        QDate current_date = QDate::currentDate();
        // 检查是否跨日，如果是，则执行删除操作并更新日期
        if (current_date != last_expired_tasks_delete_date) {
            task_list_panel->deleteExpiredTasks(settings.create_setting.auto_del_expired_tasks_cbb, current_date);
            last_expired_tasks_delete_date = current_date;
        }

    });

    // 提高计时器频率，每小时检查一次是否跨日
    expired_tasks_monitor_timer->start(60*60*1000);
}

void MainWindow::selectOptimalSlot()
{
    //尝试优先选择上一次使用的槽盒号
    auto it = std::find(m_current_slots.begin(), m_current_slots.end(), m_slots_last_index);
    if (it != m_current_slots.end())
    {
        int idx = std::distance(m_current_slots.begin(), it);
        m_slots_current_index = m_current_slots[idx];
        qDebug() << "优先选择上次任务完成的槽盒号: " << m_slots_current_index;
    }
    else
    {
        //如果上次使用的槽盒号不在当前任务列表中，则顺序选择第一个槽盒号
        m_slots_current_index = m_current_slots[0];
        qDebug() << "顺序选择第一个槽盒号:" << m_slots_current_index;
        //弹出首个槽盒
        m_current_slots.erase(m_current_slots.begin());
    }

    m_slots_last_index = m_slots_current_index;
    //转换成标刻调用的索引号，显示槽盒号-1
    m_slots_current_index -= 1;
}

void MainWindow::selectAlternatingSlot()
{
    if (m_current_slots.empty())
    {
        qDebug() << "当前任务槽盒列表为空，无法选择槽盒";
        return;
    }

    // 如果上一次用的是1，这次用2；反之亦然
    if (m_slots_last_index == 1)
        m_slots_current_index = 2;
    else
        m_slots_current_index = 1;

    // 确保选择的槽盒号在当前可用列表中
//    auto it = std::find(m_current_slots.begin(), m_current_slots.end(), m_slots_current_index);
//    if (it == m_current_slots.end())
//    {
//        // 如果交替选的槽号不在当前列表里，就选当前列表里的第一个
//        m_slots_current_index = m_current_slots.front();
//        qDebug() << "交替槽不在列表，选择第一个槽盒号:" << m_slots_current_index;
//    }
//    else
//    {
//        qDebug() << "交替选择槽盒号:" << m_slots_current_index;
//    }

    // 记录为最后一次的槽号
    m_slots_last_index = m_slots_current_index;

    // 转换成标刻调用的索引号
    m_slots_current_index -= 1;
}


void MainWindow::refreshTodayPrintNumber()
{
    QList<int> taskTypes = {};
    QDateTime sel_startTime = QDateTime::currentDateTime();
    QDateTime sel_endTime = QDateTime::currentDateTime();
    int day_print_cnt = print_task_manager.countPrintTasks(taskTypes, sel_startTime, sel_endTime);
    edit_panel->setLaserInfoText(tr("当日打印："), QString::number(day_print_cnt));
}

int MainWindow::getLastReminderCycle()
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_cycle"));
    int cycle = last_status_ini.value("cycle",0).toInt();
    last_status_ini.endGroup();
    return cycle;
}

int MainWindow::getLastReminderPrintCount()
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_print_count"));
    int print_count = last_status_ini.value("print_count",0).toInt();
    last_status_ini.endGroup();
    return print_count;
}

void MainWindow::saveLastReminderPrintCount(int count)
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_print_count"));
    last_status_ini.setValue(QString("print_count"), count);
    last_status_ini.endGroup();
}

void MainWindow::saveCurrentReminderCycle(int cycle)
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_cycle"));
    last_status_ini.setValue(QString("cycle"), cycle);
    last_status_ini.endGroup();
}

int MainWindow::getReminderCount()
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_count"));
    int count = last_status_ini.value("count",0).toInt();
    last_status_ini.endGroup();
    return count;
}

void MainWindow::saveReminderCount(int count)
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_reminder_count"));
    last_status_ini.setValue(QString("count"), count);
    last_status_ini.endGroup();
}

int MainWindow::getCurrentPrintCount()
{
    QList<int> taskTypes = {};
    QDateTime sel_startTime = QDateTime::fromString("1970-01-01",Qt::ISODate);
    QDateTime sel_endTime = QDateTime::currentDateTime();
    int total_print_cnt = print_task_manager.countPrintTasks(taskTypes, sel_startTime, sel_endTime);
    return total_print_cnt;
}

void MainWindow::checkForReminder()
{
    int reminderInterval = settings.create_setting.maintenance_remind_cbb;  // 获取设置的提醒间隔
    // 设置关闭，不提醒
    if(0 == reminderInterval)
    {
        //重置提醒次数
        saveReminderCount(0);
        return;
    }

    int printCount = getCurrentPrintCount();  // 获取当前打印次数
    //int lastReminderCycle = getLastReminderCycle();  // 获取上次启动时的提醒周期数
    int lastReminderPrintCount = getLastReminderPrintCount();  //获取上次打印提醒数

    int reminderCount = getReminderCount();  // 获取当前提醒次数

    // 计算当前打印数与上次打印提醒数之差 是否>= reminderInterval(提醒间隔数)
    int reminderDiffrent = printCount - lastReminderPrintCount;

    // 判断是否满足提醒条件：是否>= reminderInterval(提醒间隔数)，并且提醒次数小于3次
    //if (reminderDiffrent >= reminderInterval && reminderCount < 3)
    if (reminderDiffrent >= reminderInterval)
    {
        // 弹出提醒框
        QString message = QString(tr("维护提醒：打印次数超过%1次！")).arg(reminderInterval);
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(message);

        if(msg_box->Exec(2,settings.create_setting.alarm_sound_onoff_btn,m_pMarkHandle) == QDialog::Accepted)
        {
            saveLastReminderPrintCount(printCount);
        }

     }
}

// 新增的槽函数实现
void MainWindow::checkPrintState()
{
    // 检查list_auto_btn状态，如果未选中但在交替打印模式下且有提前准备好的槽盒，则继续执行
    if (!list_auto_btn->isChecked()) {
        if (edit_panel->GetAlternatingPrint() && m_pMarkHandle->getPreparedBoxIndex() != 0xFF) {
            qDebug() << "checkPrintState: 暂停状态，但有提前准备好的槽盒，继续处理";
        } else {
            return;
        }
    }

    // 检查状态机状态，只有在READY状态下才检查新任务
    if (m_printStateMachine->getState() != PrintProcessSta::READY) {
            qDebug() << "checkPrintState0=======>>>>>>>"<<m_printStateMachine->getState();
        return;
    }

    qDebug() << "checkPrintState1=======>>>>>>>"<<m_printStateMachine->getState();
    // 只检查并调度任务，其他状态通过信号自动触发
    m_taskScheduler->checkAndScheduleTask();
}

void MainWindow::handleTaskReady(const QString &taskName)
{
    QMutexLocker locker(&m_mutex);
    current_print_name = taskName;
    // 设置当前任务到TaskScheduler，避免重复调度
    m_taskScheduler->setCurrentTask(taskName);

    QSettings task_list_ini(PRINT_LIST_INI, QSettings::IniFormat);
    task_list_ini.beginGroup("TaskList");
    QString task_str = task_list_ini.value(taskName, "").toString();
    task_list_ini.endGroup();

    if (task_str.isEmpty())
    {
        m_taskScheduler->setCurrentTask(QString());
        Logger::instance().log("handleTaskReady", "未找到任务数据: " + taskName);
        m_printStateMachine->setState(PrintProcessSta::EXCEPTION);
        return;
    }

    current_print_task = task_str;

    QStringList task_text_split = task_str.split(SPLIT_SYMBOL);

    QString slots_num_str = task_text_split[TaskTextPos::TASK_SLOTS];
    QStringList slotStrings = processSlotStrings(slots_num_str);
    qDebug() << "Processed slot strings==================>>>>>>" << slotStrings;

    if(slotStrings.size() < 1)
    {
        slots_num_str = edit_panel->GetSelectSlots();
        slotStrings = slots_num_str.split(",");
    }

    m_current_slots.clear();
    for (const QString& slot : slotStrings) {
        bool ok;
        int slotNum = slot.toInt(&ok);
        if (ok && slotNum > 0) {
            m_current_slots.push_back(slotNum);
        }
    }

    if (!m_current_slots.empty())
    {
        if(edit_panel->GetAlternatingPrint())
        {
            if(!list_auto_btn->isChecked() && m_pMarkHandle->getPreparedBoxIndex() != 0xFF)
            {
                m_slots_current_index = m_pMarkHandle->getPreparedBoxIndex();
                m_pMarkHandle->setPreparedBoxIndex(0xFF);
                qDebug() << "handleTaskReady: 使用提前准备好的槽盒" << m_slots_current_index;
            }
            else
            {
                selectAlternatingSlot();
            }
        }
        else
        {
            selectOptimalSlot();
        }

        print_list_panel->setPrintTaskState(current_print_name, PrintSta::PRINT_STA_PRINTING);
        m_printStateMachine->setState(PrintProcessSta::STARTMARK);
    }
    else
    {
        m_printStateMachine->setState(PrintProcessSta::EXCEPTION);
        Logger::instance().log("handleTaskReady", "任务没有设定槽盒号！");
        emit allowDialogShow(tr("任务没有设定槽盒号！"));
    }
}

void MainWindow::handleMarkStarted()
{
    // 标刻开始的处理
    qDebug() << "Mark started";
}

void MainWindow::handleMarkFinished()
{
    // 标刻完成的处理
    qDebug() << "Mark finished";
    // 通过状态机转换到FINISHED状态
    m_printStateMachine->setState(PrintProcessSta::FINISHED);
}


void MainWindow::handleMarkError(const QString &error)
{
    // 标刻错误的处理
    qDebug() << "Mark error:" << error;
    // 通过状态机转换到EXCEPTION状态
    m_printStateMachine->setState(PrintProcessSta::EXCEPTION);
}

void MainWindow::handleMarkStateChanged(int stateNum, const QString &desc)
{
    // 标刻状态变化的处理
    qDebug() << "Mark state changed:" << stateNum << "-" << desc;
}

void MainWindow::handleStateChanged(PrintProcessSta newState, PrintProcessSta oldState)
{
    // 打印状态变化的处理
    qDebug() << "Print state changed from" << oldState << "to" << newState;
    print_process_sta = newState;
}

void MainWindow::handleTaskQueueEmpty()
{
    // 任务队列为空的处理
    qDebug() << "Task queue is empty";
    // 通过状态机转换到ENDED状态
    m_printStateMachine->setState(PrintProcessSta::ENDED);
}


PrintProcessSta MainWindow::getPrintStateMachineState() const
{
    if (m_printStateMachine)
    {
        return m_printStateMachine->getState();
    }
    return PrintProcessSta::READY;
}
