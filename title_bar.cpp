#include "title_bar.h"

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    set_layout();
    set_stylesheet();
    set_main_menu();

    connect(quit_btn, &QPushButton::clicked, this, &TitleBar::quit_btn_Clicked);

    refreshTitleBarText();
}

void TitleBar::refreshTitleBarText()
{
#ifdef USE_CASSETTE
title->setText(tr("组织盒激光书写仪"));
#elif defined(USE_SLIDE)
title->setText(tr("载玻片激光书写仪"));
#endif

menu_btn->setText(tr("菜单"));
history_option->setText(tr("历史"));
template_option ->setText(tr("模板"));
setting_option ->setText(tr("设置"));
maintenance_option ->setText(tr("维护"));
about_option ->setText(tr("关于"));
quit_option->setText(tr("退出"));
quit_btn->setText(tr("退出"));
}

void TitleBar::setMachineSta(machineSta sta)
{
    init_sta_on->hide();
    ready_sta_on->hide();
    print_sta_on->hide();
    error_sta_on->hide();

    switch (sta)
    {
       case initing:
           init_sta_on->show();
           machine_sta_text->setText(tr("初始化中"));
       break;

      case ready:
        ready_sta_on->show();
        machine_sta_text->setText(tr("就绪"));
      break;

      case printing:
         print_sta_on->show();
         machine_sta_text->setText(tr("打印中"));
      break;

      case error:
         error_sta_on->show();
         machine_sta_text->setText(tr("设备异常"));
      break;

    }
}

void TitleBar::set_layout()
{
    this->setFixedSize(800, 50);

    title = new QLabel(this);
    title->setGeometry(0, 0, 400, 50);

    //初始化
    QPixmap *init_sta_on_px = new QPixmap(18,18);
    init_sta_on_px->fill(Qt::transparent);

    QPainter painter;
    painter.begin(init_sta_on_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#f5f5f5")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    init_sta_on = new QLabel(this);
    init_sta_on->setGeometry(410, 17, 18, 18);
    init_sta_on->setPixmap(*init_sta_on_px);

    //就绪
    QPixmap *ready_sta_on_px = new QPixmap(18,18);
    ready_sta_on_px->fill(Qt::transparent);
    painter.begin(ready_sta_on_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#22ad38")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    ready_sta_on = new QLabel(this);
    ready_sta_on->setGeometry(410, 17, 18, 18);
    ready_sta_on->setPixmap(*ready_sta_on_px);

    //打印中
    QPixmap *print_sta_on_px = new QPixmap(18,18);
    print_sta_on_px->fill(Qt::transparent);
    painter.begin(print_sta_on_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#f39801")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    print_sta_on = new QLabel(this);
    print_sta_on->setGeometry(410, 17, 18, 18);
    print_sta_on->setPixmap(*print_sta_on_px);

    //设备异常
    QPixmap *error_sta_on_px = new QPixmap(18,18);
    error_sta_on_px->fill(Qt::transparent);
    painter.begin(error_sta_on_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#e70012")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    error_sta_on = new QLabel(this);
    error_sta_on->setGeometry(410, 17, 18, 18);
    error_sta_on->setPixmap(*error_sta_on_px);

    machine_sta_text = new QLabel(this);
    machine_sta_text->setGeometry(440, 0, 150, 50);

    main_menu = new QMenu(this);
    main_menu->setFixedWidth(300);

    menu_btn = new QPushButton(this);
    menu_btn->setGeometry(650, 0, 150, 50); //480, 0, 160, 50
    menu_btn->setText(tr("菜单"));
    menu_btn->setMenu(main_menu);
    QIcon icon(":/image/menu.png"); // 例如使用资源文件里的图标
    menu_btn->setIcon(icon);
    menu_btn->setLayoutDirection(Qt::RightToLeft);
    menu_btn->setIconSize(QSize(20, 20));
    menu_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击后焦点停留

    quit_btn = new QPushButton(this);
    quit_btn->setGeometry(670, 0, 130, 50);
    quit_btn->setText(tr("退出"));
    quit_btn->hide();
}

void TitleBar::set_stylesheet()
{
    this->setStyleSheet(SS_title_bar);
    title->setStyleSheet(SS_title);
    machine_sta_text->setStyleSheet(SS_machine_status);

    menu_btn->setStyleSheet(SS_menu_btn);
    main_menu->setStyleSheet(SS_main_menu);
    quit_btn->setStyleSheet(SS_quit_btn);
}

void TitleBar::set_main_menu()
{
    history_option = new QAction(tr("历史"));
    template_option = new QAction(tr("模板"));
    setting_option = new QAction(tr("设置"));
    maintenance_option = new QAction(tr("调试"));
    about_option = new QAction(tr("关于"));
    quit_option = new QAction(tr("退出"));

    main_menu->addAction(history_option);
    //main_menu->addAction(template_option);
    main_menu->addAction(setting_option);
    //main_menu->addAction(maintenance_option);
    main_menu->addAction(about_option);
    main_menu->addAction(quit_option);

    connect(history_option, &QAction::triggered, this, &TitleBar::history_option_Triggered);
    //connect(template_option, &QAction::triggered, this, &TitleBar::template_option_Triggered);
    connect(setting_option, &QAction::triggered, this, &TitleBar::setting_option_Triggered);
    connect(maintenance_option, &QAction::triggered, this, &TitleBar::maintenance_option_Triggered);
    connect(about_option, &QAction::triggered, this, &TitleBar::about_option_Triggered);
    connect(quit_option, &QAction::triggered, this, &TitleBar::quit_option_Triggered);

    setMachineSta(initing);
}

