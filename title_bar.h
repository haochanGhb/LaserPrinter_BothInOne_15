#ifndef TITLE_BAR_H
#define TITLE_BAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QPainter>
#include <QApplication>

#include "stylesheet.h"

enum machineSta{
    initing,  //初始化中
    ready, //就绪
    printing, //打印中
    error//设备异常
};

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);

    void refreshTitleBarText();
    void setMachineSta(machineSta sta);
private:
    QLabel *title;

    QLabel *init_sta_on;
    QLabel *ready_sta_on;
    QLabel *print_sta_on;
    QLabel *error_sta_on;

    QLabel *machine_sta_text;

    QPushButton *menu_btn;
    QPushButton *quit_btn;
    QMenu *main_menu;

    QAction *history_option;
    QAction *template_option;
    QAction *setting_option;
    QAction *maintenance_option;
    QAction *about_option;
    QAction *quit_option;

    void set_layout();
    void set_stylesheet();
    void set_main_menu();
signals:
    void history_option_Triggered();
    void template_option_Triggered();
    void setting_option_Triggered();
    void maintenance_option_Triggered();
    void about_option_Triggered();
    void quit_option_Triggered();

    void quit_btn_Clicked();

private slots:

};

#endif // TITLE_BAR_H
