#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QPointer>

/*!
 * \brief The SocketThread - класс для запуска в отдельном потоке, для того, чтобы
 * с сервера можно было отправлять данные в сокеты многопоточно
 */
class SocketThread : public QObject
{
    Q_OBJECT
public:
    explicit SocketThread(int socketDescriptor, QObject *parent = nullptr);
    ~SocketThread() override;
public slots:
    void close() {m_socket->close();}
    void setSocketDescriptor(int socketDescriptor);
    void onThreadStarted();
    void sendData(QList<QByteArray> data);
signals:
    void gotResponse(const QString& ip, const QByteArray& data);
    void disconnected(const QString& ip);
private slots:
    void readSocket();
private:
    int                     m_socketDescriptor;
    QPointer<QTcpSocket>    m_socket;
};

