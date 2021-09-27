#pragma once

#include <QObject>
#include <QPointer>
#include <QMqttClient>
#include <QMqttTopicName>
#include <QMqttSubscription>
#include <QMqttTopicFilter>
#include <QThread>

#include "IskraTcpServer.h"
#include "RouteInfoTemplate.h"
#include "MqttClientHandler.h"
#include "Global.h"

class AbstractWindowTemplate;
class IskraSimpleCommand;
class LedController;

using namespace Global::Envirenment;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = nullptr);
    ~MainController() override;
signals:
    void subscribe(QMqttTopicFilter filter);
private slots:
    void onLedConnected(const QString& deviceIp);
    void onLedResponse(const QString& deviceIP, const QByteArray &response);
    void onDataForLedReady(const QString& deviceIP, QList<QByteArray> data, CurrentTableInfo info, bool isWeather);
    void onLedOnlineStatus(const QString& deviceIP, bool isOnline);
    void onMqttMsgReceived(QMqttMessage msg);    
private:    
    void publishDiagnosticInfo(const QString& ip, bool isOnline);    
    void createSubscription(const QString& deviceIP);
    /*!
     * \brief publishCurrentTableShowInfo - публикует текущую информацию, отображаемую на табло
     * \param ip адресс табло
     */
    void publishCurrentTableShowInfo(const QString &ip, CurrentTableInfo info);
    void run();
private:
    QThread*                                m_mqttWorkerThread;
    QPointer<IskraTcpServer>                m_server;
    QPointer<MqttClientHandler>             m_mqttClientHandler;
    /*!
     * \brief m_ledControllersMap - ip табло и его контроллер, отвечающий за отображение
     */
    QMap<QString, QPointer<LedController>>  m_ledControllersMap;
    QString                                 m_taskTopicPrefix;
    QString                                 m_emergencyTopic;
    QString                                 m_weatherTopic;
    QString                                 m_diagnosticTopic;
    QString                                 m_currentTableInfoTopic;
};

