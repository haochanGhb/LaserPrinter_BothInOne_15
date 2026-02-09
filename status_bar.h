#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMenu>
#include <QTimer>
#include <QPainter>

#include "stylesheet.h"

class StatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);
    void refreshStatusBarText();
    void setRemotePrintLinkSta(bool sta);
    void setSyncPrintLinkSta(bool sta);
    void setStatetext(const QString & textInfo);
    void setStatetext2(const QString & textInfo);
private:
    QPushButton *datetime_btn;
    QTimer *datetime_btn_click_timer;
    int clickCount;

    QLabel *stateText_Label;
    QLabel *stateText2_Label;
    QTimer *timer;
    QLabel *remote_sta_on;
    QLabel *remote_sta_off;
    QLabel *sync_sta_on;
    QLabel *sync_sta_off;
    QLabel *remote_sta_text;
    QLabel *sync_sta_text;

    void set_layout();
    void set_stylesheet();

signals:
    void datetime_btn_multiple_Clicked();
    void signal_datetime_update();
private slots:
    void update_time();
    void on_datetime_btn_clicked();

};

#endif // STATUS_BAR_H
