#include "DeviceDiscovery.h"
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QTimer>

static const QByteArray DISCOVER_MAGIC = "GR_DISCOVER_DEVICE_V1";


DeviceDiscovery::DeviceDiscovery(QObject *parent) : QObject(parent)
{
    m_udp = new QUdpSocket(this);
    connect(m_udp, &QUdpSocket::readyRead,
            this, &DeviceDiscovery::onReadyRead);
}

DeviceDiscovery::~DeviceDiscovery()
{
    m_udp->close();
}

void DeviceDiscovery::setDiscoveryEnabled(bool enable)
{
    m_enable = enable;
}

bool DeviceDiscovery::startDevice(quint16 discoverPort,
                                  const QString &deviceName,
                                  quint16 servicePort)
{
    m_isDevice     = true;
    m_discoverPort = discoverPort;
    m_deviceName   = deviceName;
    m_servicePort  = servicePort;

    return m_udp->bind(discoverPort,
                       QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

void DeviceDiscovery::setDeviceName(const QString &deviceName)
{
    m_deviceName = deviceName;
}

void DeviceDiscovery::setServicePort(quint16 servicePort)
{
    m_servicePort = servicePort;
}

void DeviceDiscovery::startDiscovery(quint16 discoverPort, int timeoutMs)
{
    m_isDevice     = false;
    m_discoverPort = discoverPort;
    m_foundSet.clear();

    m_udp->bind(0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    sendDiscovery();

    QTimer::singleShot(timeoutMs, this, [=]() {
        m_udp->close();
    });
}

void DeviceDiscovery::sendDiscovery()
{
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces())
    {
        if (!(iface.flags() & QNetworkInterface::IsUp))
            continue;

        for (const QNetworkAddressEntry &entry : iface.addressEntries())
        {
            if (entry.broadcast().isNull())
                continue;

            m_udp->writeDatagram(
                DISCOVER_MAGIC,
                entry.broadcast(),
                m_discoverPort
            );
        }
    }
}

void DeviceDiscovery::onReadyRead()
{
    while (m_udp->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_udp->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        m_udp->readDatagram(datagram.data(), datagram.size(),
                            &sender, &senderPort);
        qDebug() << "Datagram" << datagram;

        if (!m_enable)
            return;

        /* 设备端收到发现请求 */
        if (m_isDevice && datagram == DISCOVER_MAGIC)
        {
            emit discoveryRequest(sender, senderPort);
            replyDeviceInfo(sender, senderPort);
            continue;
        }

        /* 客户端收到设备信息 */
        if (!m_isDevice)
        {
            QJsonDocument doc = QJsonDocument::fromJson(datagram);
            if (!doc.isObject())
                continue;

            QJsonObject obj = doc.object();
            if (obj["cmd"].toString() != "DEVICE_INFO")
                continue;

            DeviceInfo info;
            info.name = obj["name"].toString();
            info.port = static_cast<quint16>(obj["port"].toInt());
            info.sn   = obj["sn"].toString();
            info.ip   = QHostAddress(sender.toIPv4Address()).toString();

            QString key = info.ip + ":" + QString::number(info.port);
            if (m_foundSet.contains(key))
                continue;

            m_foundSet.insert(key);
            emit deviceFound(info);
        }
    }
}

void DeviceDiscovery::replyDeviceInfo(const QHostAddress &addr, quint16 port)
{
    QJsonObject obj;
    obj["cmd"]  = "DEVICE_INFO";
    obj["name"] = m_deviceName;
    obj["port"] = m_servicePort;
    obj["sn"]   = "SN-001";

    QByteArray data =
        QJsonDocument(obj).toJson(QJsonDocument::Compact);

    m_udp->writeDatagram(data, addr, port);
}
