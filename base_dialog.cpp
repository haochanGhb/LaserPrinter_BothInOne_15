#include "base_dialog.h"
#include "base_dialog_style.h"

BaseDialog::BaseDialog(QDialog *parent) : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    this->setGeometry(0,0,800,1280);

    create_widget();
    setup_layout();
    set_stylesheet();

    connect(back_btn, &QPushButton::clicked, this, &BaseDialog::on_back_btn_clicked);
}

void BaseDialog::setTitle(QString Title)
{
    title->setText(Title);
}

void BaseDialog::mousePressEvent(QMouseEvent *event)
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

void BaseDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && isDraggable)
    {
        //move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void BaseDialog::mouseReleaseEvent(QMouseEvent *event)
{
    isDraggable = false;
}

void BaseDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        event->ignore();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}

void BaseDialog::startAnimation()
{
    // 设置初始位置为屏幕左边界外
    int startX = 0;
    int endX = 0;  // 最终位置

    // 设置动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);  // 动画持续时间为500毫秒
    animation->setStartValue(QRect(startX, 0, 0, height()));
    animation->setEndValue(QRect(endX, 0, width(), height()));
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    // 开始动画
    animation->start();
}

void BaseDialog::backAnimation()
{
    // 设置初始位置为屏幕左边界外
    int startX = 0;
    int endX = 0;  // 最终位置

    // 设置动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);  // 动画持续时间为500毫秒
    animation->setStartValue(QRect(startX, 0, width(), height()));
    animation->setEndValue(QRect(endX, 0, 0, height()));
    animation->setEasingCurve(QEasingCurve::OutCubic);

    // 开始动画
    animation->start();

}

void BaseDialog::create_widget()
{
    title_bar = new QWidget(this);
    title_bar->setGeometry(0, 0, 800, 50); //(0, 50, 740, 60);

    title = new QLabel(title_bar);
    title->setGeometry(0,0,800,50);

    back_btn = new QPushButton(tr("返回"), this);
    back_btn->setGeometry(670,0,130,50);  //(740,50,60,60);
    back_btn->setFocusPolicy(Qt::NoFocus);

    QIcon icon(":/image/back.png"); // 例如使用资源文件里的图标
    back_btn->setIcon(icon);
    back_btn->setLayoutDirection(Qt::RightToLeft);
    back_btn->setIconSize(QSize(20, 20));

    datetime_btn = new QPushButton(this);
    datetime_btn->setGeometry(630,1230,170,50);
    datetime_btn->setFixedHeight(50);
    datetime_btn->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    main_area = new QWidget();
    main_scroll_area = new QScrollArea(this);
    main_scroll_area->setGeometry(0, 50, 800, 1100); //(0, 110, 740, 1100);
    main_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    main_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    main_scroll_area->setWidgetResizable(true);
    main_scroll_area->setWidget(main_area);

    bottom_bar = new QWidget(this);
    bottom_bar->setGeometry(0, 1150, 800, 80); //(0, 1210, 740, 70);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BaseDialog::update_time);
    timer->start(1000);
}

void BaseDialog::setup_layout()
{
    main_layout = new QVBoxLayout(main_area);
    main_layout->setContentsMargins(2,10,2,10); //(2,10,2,10)
    main_layout->setSpacing(10);

    bottom_bar_layout = new QHBoxLayout(bottom_bar);
    bottom_bar_layout->setContentsMargins(10,10,10,10); //(10,10,10,10)
    bottom_bar_layout->setSpacing(10);

}

void BaseDialog::set_stylesheet()
{
    this->setStyleSheet(SS_BaseDialog);

    title_bar->setStyleSheet(SS_title_bar);
    title->setStyleSheet(SS_title);
    back_btn->setStyleSheet(SS_back_btn);
    datetime_btn->setStyleSheet(SS_datetime_btn);

    main_area->setStyleSheet(SS_main_area);
    bottom_bar->setStyleSheet(SS_bottom_bar);
}

void BaseDialog::on_back_btn_clicked()
{
    timer->stop();
    this->close();
}

void BaseDialog::update_time()
{
    datetime_btn->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void BaseDialog::bottomBar_hide()
{
   main_scroll_area->setGeometry(0, 50, 800, 1180);
   bottom_bar->hide();
}

void BaseDialog::dialog_show()
{
//    this->show();
//    QTimer::singleShot(10,this,&BaseDialog::startAnimation);
}

void BaseDialog::dialog_exit()
{
//    backAnimation();
//    QTimer::singleShot(300,this,&BaseDialog::close);

//    this->close();
//    this->deleteLater();
}
