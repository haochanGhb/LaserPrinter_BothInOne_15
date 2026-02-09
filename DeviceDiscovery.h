#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QJsonObject>


struct DeviceInfo
{
    QString name;
    QString ip;
    quint16 port;
    QString sn;
};

class DeviceDiscovery : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDiscovery(QObject *parent = nullptr);
    ~DeviceDiscovery();

    void setDiscoveryEnabled(bool enable);

    /* 设备端 */
    bool startDevice(quint16 discoverPort,
                     const QString &deviceName,
                     quint16 servicePort);

    void setDeviceName(const QString &deviceName);
    void setServicePort(quint16 servicePort);

    /* 客户端 */
    void startDiscovery(quint16 discoverPort, int timeoutMs = 1000);

signals:
    /* 客户端 */
    void deviceFound(const DeviceInfo &info);

    /* 设备端 */
    void discoveryRequest(const QHostAddress &from, quint16 port);

private slots:
    void onReadyRead();

private:
    void sendDiscovery();
    void replyDeviceInfo(const QHostAddress &addr, quint16 port);

private:
    QUdpSocket *m_udp = nullptr;
    bool m_enable = true;

    bool     m_isDevice = false;
    quint16  m_discoverPort = 0;

    QString  m_deviceName;
    quint16  m_servicePort;

    quint16 test;

    QSet<QString> m_foundSet;   // IP:PORT 去重
};

#endif // DEVICEDISCOVERY_H
