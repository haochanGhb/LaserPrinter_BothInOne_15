#include "DeviceDiscoveryDialog.h"
#include "dialogbox_style.h"
#include <QRegExpValidator>
#include "public_fun.h"
#include <QTimer>

DeviceDiscoveryDialog::DeviceDiscoveryDialog(QDialog *parent) : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    create_widget();
    set_stylesheet();

    initDeviceDiscovery();

    connect(close_btn, &QPushButton::clicked, this, &DeviceDiscoveryDialog::on_close_btn_clicked);
    connect(ok_btn, &QPushButton::clicked, this, &DeviceDiscoveryDialog::on_ok_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &DeviceDiscoveryDialog::on_cancel_btn_clicked);
    connect(m_pBtnRefresh, &QPushButton::clicked, this, &DeviceDiscoveryDialog::on_refresh_btn_clicked);

    on_refresh_btn_clicked();

}

void DeviceDiscoveryDialog::searchDevice(quint16 discoverPort)
{
    m_pDeviceDiscovery->startDiscovery(discoverPort, 2000);
}

DeviceInfo DeviceDiscoveryDialog::getSelectedDeviceInfo()
{
    return m_selectedDeviceInfo;
}

void DeviceDiscoveryDialog::create_widget()
{
    this->setFixedSize(360,460);
    close_btn = new QPushButton(this);
    close_btn->setGeometry(310,10,40,40);
    close_btn->setFocusPolicy(Qt::NoFocus);

    m_pTitleLabel = new QLabel(this);
//    m_pTitleLabel->setGeometry(40, 0, 200, 60);
    m_pTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pTitleLabel->setText(tr("设备查找"));
    m_pTitleLabel->setFixedHeight(60);
    m_pTitleLabel->move(60,0);

    m_pDeviceListWidget = new QListWidget(this);
    m_pDeviceListWidget->setGeometry(20,60,320,320);
    m_pDeviceListWidget->setDragEnabled(false);
    m_pDeviceListWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    ok_btn = new QPushButton(this);
    ok_btn->setGeometry(40,400,120,35);
    ok_btn->setText(tr("确定"));
//    ok_btn->setFocusPolicy(Qt::StrongFocus);
//    ok_btn->setDefault(true);
//    ok_btn->setFocus();
    ok_btn->setShortcut(QKeySequence(Qt::Key_Enter));

    cancel_btn = new QPushButton(this);
    cancel_btn->setGeometry(200,400,120,35);
    cancel_btn->setText(tr("取消"));


    m_pBtnRefresh = new QPushButton(this);
    m_pBtnRefresh->setGeometry(20,10,40,40);
    m_pBtnRefresh->setFocusPolicy(Qt::NoFocus);
    m_pBtnRefresh->setText("⭮");

}

void DeviceDiscoveryDialog::set_stylesheet()
{
    this->setStyleSheet(SS_DialogBox);

    close_btn->setStyleSheet(SS_close_btn);
    ok_btn->setStyleSheet(SS_Primary_Dlg_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Dlg_Btn);

    m_pTitleLabel->setStyleSheet(SS_title_label);
    m_pDeviceListWidget->setStyleSheet(SS_ListWidget);


    m_pBtnRefresh->setStyleSheet(SS_refresh_btn);

}

void DeviceDiscoveryDialog::initDeviceDiscovery()
{
    m_pDeviceDiscovery = new DeviceDiscovery(this);
    connect(m_pDeviceDiscovery, &DeviceDiscovery::deviceFound,
                     [=](const DeviceInfo &info) {
        qDebug() << "FOUND:" << info.name << info.ip << info.port;

        QString devInfoText = info.name + "(" + info.ip + ":" + QString::number(info.port) + ")";

        QListWidgetItem *item = new QListWidgetItem(devInfoText);
        item->setData(Qt::UserRole, QStringList{info.name, info.ip, QString::number(info.port)});
        m_pDeviceListWidget->addItem(item);
    });
}

void DeviceDiscoveryDialog::on_close_btn_clicked()
{
    this->close();
    this->deleteLater();
}

void DeviceDiscoveryDialog::on_ok_btn_clicked()
{
    if(nullptr != m_pDeviceListWidget->currentItem())
    {
        QStringList infoList = m_pDeviceListWidget->currentItem()->data(Qt::UserRole).toStringList();

        m_selectedDeviceInfo.name = infoList[0];
        m_selectedDeviceInfo.ip = infoList[1];
        m_selectedDeviceInfo.port = infoList[2].toUInt();
    }
    else
    {
        m_selectedDeviceInfo.name = "";
        m_selectedDeviceInfo.ip = "";
        m_selectedDeviceInfo.port = 0;
    }

    this->accept();
    this->deleteLater();
}

void DeviceDiscoveryDialog::on_cancel_btn_clicked()
{
    this->reject();
    this->deleteLater();
}

void DeviceDiscoveryDialog::on_refresh_btn_clicked()
{
    m_pDeviceListWidget->clear();
    m_pBtnRefresh->setEnabled(false);
    m_pBtnRefresh->setText("...");

    searchDevice(SCANNER_RECEIVER_DISCOVER_PORT);

    QTimer::singleShot(2000, this, [=]() {
        m_pBtnRefresh->setEnabled(true);
        m_pBtnRefresh->setText("⭮");
    });
}

void DeviceDiscoveryDialog::mousePressEvent(QMouseEvent *event)
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

void DeviceDiscoveryDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && isDraggable)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void DeviceDiscoveryDialog::mouseReleaseEvent(QMouseEvent *event)
{
    isDraggable = false;
}

void DeviceDiscoveryDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter)
    {
        //event->ignore();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}
