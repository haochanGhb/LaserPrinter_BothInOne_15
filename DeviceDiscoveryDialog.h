#ifndef DEVICE_DISCOVERY_DIALOG_H
#define DEVICE_DISCOVERY_DIALOG_H


#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QTextEdit>
#include <QDebug>
#include <QScrollArea>
#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QListWidget>

#include "DeviceDiscovery.h"

class DeviceDiscoveryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceDiscoveryDialog(QDialog *parent = nullptr);
    void searchDevice(quint16 discoverPort);

    DeviceInfo m_selectedDeviceInfo;
    DeviceInfo getSelectedDeviceInfo();

private:

    DeviceDiscovery *m_pDeviceDiscovery;

    QLabel *m_pTitleLabel;
    QListWidget *m_pDeviceListWidget;

    QPushButton *ok_btn;
    QPushButton *cancel_btn;
    QPushButton *close_btn;

    QPushButton *m_pBtnRefresh;

    void create_widget();
    void set_stylesheet();

    void initDeviceDiscovery();

signals:

private slots:
    void on_close_btn_clicked();
    void on_ok_btn_clicked();
    void on_cancel_btn_clicked();
    void on_refresh_btn_clicked();

protected:
    QPoint dragPosition;
    bool isDraggable = false;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
};

#endif // DEVICE_DISCOVERY_DIALOG_H
