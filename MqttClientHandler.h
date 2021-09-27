#pragma once

#include <QObject>
#include <QMqttClient>
#include <QMqttSubscription>
#include <QMqttMessage>
#include <QProcessEnvironment>
#include <QMutex>

class MqttClientHandler : public QObject
{
    Q_OBJECT
public:
    MqttClientHandler(QObject* parent = nullptr);
public slots:
    QMqttMessage getLastMsg(const QString& topic);
    void subscribe(const QMqttTopicFilter& filter);
    void publish(const QString& topic, const QByteArray& payload, bool retain = false);
signals:
    void connected();
    void msgReceived(QMqttMessage msg);
public slots:
    void onThreadStarted();
private:
    void getEnvirenmentVariables();
private:
    QString                                 m_host;
    quint16                                 m_port;
    quint16                                 m_keepAlive;
    QMqttClient*                            m_mqttClient;
    QList<QMqttTopicFilter>                 m_filters;
    QMap<QMqttTopicFilter, QMqttMessage>    m_lastMessages;
    QMutex                                  m_lastMessageMutex;
};

