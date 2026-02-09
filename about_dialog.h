#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "base_dialog.h"
#include <QWidget>
#include "setting_dialog.h"

class MainWindow;

class AboutDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
private:
    // MainWindow指针，用于访问主窗口状态
    MainWindow *mainWindow;

    QWidget *about_panel;
    QLabel *logo;
    QLabel *barcode;
    QLabel *website;
    QLabel *device_name;
    QLabel *application_version;
    QLabel *firmware_version;
    QLabel * license_notice;

    void create_widget();
    void setup_layout();
    void set_stylesheet();
    void connect_signals_and_slots();
    void set_about_info();

};

#endif // ABOUTDIALOG_H
