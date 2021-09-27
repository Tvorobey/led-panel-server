#include "SocketThread.h"

#include <QDebug>
#include <QThread>

SocketThread::SocketThread(int socketDescriptor, QObject *parent)
    : QObject(parent),
      m_socketDescriptor(socketDescriptor)
{}

SocketThread::~SocketThread()
{}

void SocketThread::setSocketDescriptor(int socketDescriptor)
{
    if (m_socketDescriptor != socketDescriptor)
    {
        qDebug() << QString("[%1] - старый дескриптор %2, новый - %3").arg(m_socket->peerAddress().toString(),
                                                                           m_socketDescriptor, socketDescriptor);

        m_socketDescriptor = socketDescriptor;

        if (!m_socket->setSocketDescriptor(m_socketDescriptor))
        {
            qWarning() << "Cant set socket descriptor for led " << m_socket->errorString();
            return;
        }
    }
}

void SocketThread::onThreadStarted()
{
    m_socket = new QTcpSocket();

    if (!m_socket->setSocketDescriptor(m_socketDescriptor))
    {
        qWarning() << "Cant set socket descriptor for led " << m_socket->errorString();
        return;
    }

    const QString peerAddress = m_socket->peerAddress().toString();

    qDebug() << QString("Новое подключение. Адресс %1").arg(peerAddress);

    connect(m_socket, &QTcpSocket::readyRead, this, &SocketThread::readSocket);
    connect(m_socket, &QTcpSocket::disconnected, this, [this, peerAddress]()
    {
        emit disconnected(peerAddress);
    });
}

void SocketThread::sendData(QList<QByteArray> data)
{
    qDebug() << QString("Отправляем данные в табло %1").arg(m_socket->peerAddress().toString());

    qDebug() << " -->> " << "Размер: " << data.size(); // << " : " << data.toHex(' ').toUpper();

    for (auto it = data.begin(); it < data.end(); ++it)
    {
        m_socket->write(*it);
        m_socket->waitForBytesWritten();
    }
}

void SocketThread::readSocket()
{
    QByteArray response = m_socket->readAll();

    emit gotResponse(m_socket->peerAddress().toString(), response);
}
