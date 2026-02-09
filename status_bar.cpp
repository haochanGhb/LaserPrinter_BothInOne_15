#include "status_bar.h"

StatusBar::StatusBar(QWidget *parent) : QWidget(parent)
{
    set_layout();
    set_stylesheet();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::update_time);
    timer->start(1000);
}

void StatusBar::refreshStatusBarText()
{
    remote_sta_text->setText(tr("远程"));
    sync_sta_text->setText(tr("同步"));
}

void StatusBar::setRemotePrintLinkSta(bool sta)
{
    remote_sta_on->setVisible(sta);
    remote_sta_off->setVisible(!sta);
}

void StatusBar::setSyncPrintLinkSta(bool sta)
{
    sync_sta_on->setVisible(sta);
    sync_sta_off->setVisible(!sta);
}

void StatusBar::setStatetext(const QString & textInfo)
{
   stateText_Label->setText(textInfo);
}

void StatusBar::setStatetext2(const QString & textInfo)
{
   stateText2_Label->setText(textInfo);
}

void StatusBar::set_layout()
{
    this->setFixedSize(800, 50);
    this->setContentsMargins(0,0,0,0);

    QPixmap *sta_on = new QPixmap(18,18);
    sta_on->fill(Qt::transparent);

    QPixmap *sta_off = new QPixmap(18,18);
    sta_off->fill(Qt::transparent);

    QPainter painter;
    painter.begin(sta_on);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#22ad38")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    painter.begin(sta_off);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#e8e8e8")));
    painter.drawEllipse(0,0,18,18);
    painter.setBrush(QColor(QString("#f5f5f5")));
    painter.drawEllipse(3,3,12,12);
    painter.end();

    remote_sta_on = new QLabel(this);
    remote_sta_off = new QLabel(this);
    sync_sta_on = new QLabel(this);
    sync_sta_off = new QLabel(this);

    remote_sta_on->setPixmap(*sta_on);
    remote_sta_off->setPixmap(*sta_off);
    sync_sta_on->setPixmap(*sta_on);
    sync_sta_off->setPixmap(*sta_off);

    remote_sta_on->setVisible(false);
    remote_sta_off->setVisible(true);
    sync_sta_on->setVisible(false);
    sync_sta_off->setVisible(true);

    remote_sta_on->setFixedSize(18,18);
    remote_sta_off->setFixedSize(18,18);
    sync_sta_on->setFixedSize(18,18);
    sync_sta_off->setFixedSize(18,18);

    remote_sta_text = new QLabel(tr("远程"),this);
    sync_sta_text = new QLabel(tr("同步"),this);

    stateText_Label  = new QLabel(this);
    stateText_Label->setFixedHeight(50);
    stateText_Label->hide();

    stateText2_Label  = new QLabel(this);
    stateText2_Label->setFixedHeight(50);
    stateText2_Label->hide();

    datetime_btn = new QPushButton(this);
    datetime_btn->setFixedHeight(50);

    datetime_btn_click_timer = new QTimer(this);
    clickCount = 0;
    connect(datetime_btn_click_timer, &QTimer::timeout, this, [=] { clickCount = 0; });
    connect(datetime_btn, &QPushButton::clicked, this, &StatusBar::on_datetime_btn_clicked);

    QHBoxLayout *status_bar_h_layout = new QHBoxLayout(this);
    status_bar_h_layout->setContentsMargins(10,0,10,0);
    status_bar_h_layout->setSpacing(10);
    //status_bar_h_layout->addWidget(stateText_Label);
    //status_bar_h_layout->addWidget(stateText2_Label);
    status_bar_h_layout->addStretch();
    status_bar_h_layout->addWidget(remote_sta_on, 0, Qt::AlignVCenter);
    status_bar_h_layout->addWidget(remote_sta_off, 0, Qt::AlignVCenter);
    status_bar_h_layout->addWidget(remote_sta_text, 0, Qt::AlignVCenter);
    status_bar_h_layout->addSpacing(20);
    status_bar_h_layout->addWidget(sync_sta_on, 0, Qt::AlignVCenter);
    status_bar_h_layout->addWidget(sync_sta_off, 0, Qt::AlignVCenter);
    status_bar_h_layout->addWidget(sync_sta_text, 0, Qt::AlignVCenter);
    //加10个弹簧
    for(signed char i = 0;i < 10;i++)
    {
        status_bar_h_layout->addStretch();
    }
    status_bar_h_layout->addWidget(datetime_btn);

}

void StatusBar::set_stylesheet()
{
    this->setStyleSheet(SS_status_area);
    datetime_btn->setStyleSheet(SS_datetime_btn);
}

void StatusBar::update_time()
{
    datetime_btn->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    emit signal_datetime_update();
}

void StatusBar::on_datetime_btn_clicked()
{
    if(!datetime_btn_click_timer->isActive())
    {
        datetime_btn_click_timer->start(500);
    }
    clickCount++;

    if(clickCount < 3)
    return ;

    emit datetime_btn_multiple_Clicked();
}
