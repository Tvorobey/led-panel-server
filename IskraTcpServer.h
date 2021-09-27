#pragma once

//Qt
#include <QObject>
#include <QScopedPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

//STL
#include <mutex>

#include "SocketThread.h"

class IskraTcpServer : public QObject
{
    Q_OBJECT
public:
    static IskraTcpServer* getInstance();
    void sendData(const QString address, const QList<QByteArray> data);
    void closeSocket(const QString& ip);
signals:
    void gotResponse(const QString& deviceIP, const QByteArray response);
    void connectToDevice(const QString deviceIP);
    void ledDisconnected(const QString& deviceIP);
private slots:
    void run();    
    void deleteOnDisconnect();
private:
    explicit IskraTcpServer(const QString &address, const int port, QObject *parent = nullptr);
    IskraTcpServer(const IskraTcpServer& other) = delete;
    IskraTcpServer& operator = (const IskraTcpServer& other) = delete;
private:
    static std::once_flag                   m_flag;
    static QScopedPointer<IskraTcpServer>   m_instance;
    QScopedPointer<QTcpServer>              m_server;
    QMap<QString, SocketThread*>            m_socketsMap;
    QMap<QString, QThread*>                 m_threadsMap;
    bool                                    m_isConnected;
};


