//Local
#include "IskraTcpServer.h"
#include "Global.h"

//Qt
#include <QDebug>
#include <QProcessEnvironment>

using namespace Global::Envirenment;

QScopedPointer<IskraTcpServer>  IskraTcpServer::m_instance;
std::once_flag                  IskraTcpServer::m_flag;

IskraTcpServer *IskraTcpServer::getInstance()
{
    QProcessEnvironment variables = QProcessEnvironment::systemEnvironment();

    QString host = variables.value(SERVER_ADDRESS).isEmpty() ? "192.168.0.17" : variables.value(SERVER_ADDRESS);
    int port = variables.value(SERVER_PORT).isEmpty() ? 3333 : variables.value(SERVER_PORT).toInt();

    std::call_once(IskraTcpServer::m_flag, [&]()
    {
        m_instance.reset(new IskraTcpServer(host, port));
    });

    return m_instance.data();
}

void IskraTcpServer::sendData(const QString address, const QList<QByteArray> data)
{
    QMetaObject::invokeMethod(m_socketsMap.value(address), "sendData", Q_ARG(QList<QByteArray>, data));
}

void IskraTcpServer::closeSocket(const QString &ip)
{
    QMetaObject::invokeMethod(m_socketsMap[ip], "close");
}

IskraTcpServer::IskraTcpServer(const QString& address, const int port, QObject *parent)
    : QObject(parent),
      m_server(new QTcpServer(this)),
      m_isConnected(false)
{
    connect(m_server.data(), &QTcpServer::newConnection, this, &IskraTcpServer::run);
    if (!m_server->listen(QHostAddress(address), port))
    {
        qDebug() << QString("Ошибка подключения к хосту: %1, Адресс: %2, Порт: %3")
                                    .arg(m_server->errorString(), address, QString::number(port));
    }
    else
    {
        qDebug() << QString("Сервер успешно запущен. Адресс: %1. Порт: %2")
                                   .arg(address, QString::number(port));
    }
}

void IskraTcpServer::run()
{
    while (m_server->hasPendingConnections())
    {
        auto* socket = m_server->nextPendingConnection();

        QString ip = socket->peerAddress().toString();

        // Уже есть поток для этого подключения, просто подменим дескриптор
        if (m_threadsMap.contains(ip))
        {
            QMetaObject::invokeMethod(m_socketsMap[ip], "setSocketDescriptor", Q_ARG(int, socket->socketDescriptor()));

            // Ну и оповестим всех
            emit connectToDevice(ip);
        }
        else
        {
            m_threadsMap.insert(ip, new QThread(this));
            m_socketsMap.insert(ip, new SocketThread(socket->socketDescriptor()));

            m_socketsMap[ip]->moveToThread(m_threadsMap[ip]);
        }

        connect(m_threadsMap[ip], &QThread::started, this, [this, ip]()
        {
           QMetaObject::invokeMethod(m_socketsMap[ip], "onThreadStarted");

           emit connectToDevice(ip);
        });

        connect(m_threadsMap[ip], &QThread::finished, this, [this, ip]()
        {
            m_socketsMap[ip]->deleteLater();
            m_socketsMap.remove(ip);
            m_threadsMap.remove(ip);

            emit ledDisconnected(ip);
        });
        connect(m_socketsMap[ip], &SocketThread::gotResponse, this, &IskraTcpServer::gotResponse);
        connect(m_socketsMap[ip], &SocketThread::disconnected, this, [this, ip]()
        {
            qDebug() << QString("[%1] - закрыли сокет").arg(ip);
            m_threadsMap[ip]->exit();
            m_threadsMap[ip]->wait();
        });

        m_threadsMap[ip]->start(QThread::HighPriority);
    }
}

void IskraTcpServer::deleteOnDisconnect()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());

    qDebug() << QString("Сокет отсоединен. PeerAddress: %1, PeerPort: %2")
                               .arg(socket->peerAddress().toString()).arg(socket->peerPort());

    m_socketsMap.remove(socket->peerAddress().toString());

    socket->deleteLater();
}
