#include "MainController.h"
#include "AbstractWindowTemplate.h"
#include "IskraSimpleCommand.h"
#include "LedController.h"
#include "Parsers/TopicRouteInfoParser.h"
#include "Parsers/LedDiagnosticTopic.h"
#include "Parsers/WeatherTopicParser.h"
#include "Parsers/CurrentTableInfoTopic.h"

#include <QTimer>
#include <QThread>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QTimeZone>

namespace
{
    const QString SETTING_FILES_PATH = "home/settings.json";
}

MainController::MainController(QObject *parent)
    : QObject(parent)
{
    QProcessEnvironment variables = QProcessEnvironment::systemEnvironment();
    m_taskTopicPrefix = variables.value(TASK_TOPIC).isEmpty() ? "TASK/" : variables.value(TASK_TOPIC);
    m_emergencyTopic = variables.value(EMERGENCY_TOPIC).isEmpty() ? "EMERGENCY/" : variables.value(EMERGENCY_TOPIC);
    m_weatherTopic = variables.value(WEATHER_TOPIC).isEmpty() ? "WEATHER" : variables.value(WEATHER_TOPIC);
    m_diagnosticTopic = variables.value(DIAGNOSTIC_TOPIC).isEmpty() ? "DIAGNOSTIC/LED/" : variables.value(DIAGNOSTIC_TOPIC);
    m_currentTableInfoTopic = variables.value(CURRENT_TABLE_INFO_TOPIC).isEmpty() ? "CURRENT_INFO/" : variables.value(CURRENT_TABLE_INFO_TOPIC);

    m_mqttWorkerThread = new QThread(this);
    m_mqttClientHandler = new MqttClientHandler();
    m_mqttClientHandler->moveToThread(m_mqttWorkerThread);

    connect(m_mqttWorkerThread, &QThread::started, m_mqttClientHandler, &MqttClientHandler::onThreadStarted);
    connect(m_mqttWorkerThread, &QThread::finished, m_mqttClientHandler, &MqttClientHandler::deleteLater);
    connect(m_mqttClientHandler, &MqttClientHandler::connected, this, &MainController::run);
    connect(m_mqttClientHandler, &MqttClientHandler::msgReceived, this, &MainController::onMqttMsgReceived);
    connect(this, &MainController::subscribe, m_mqttClientHandler, &MqttClientHandler::subscribe);
    m_mqttWorkerThread->start();
}

MainController::~MainController()
{
    m_mqttWorkerThread->exit();
    m_mqttWorkerThread->wait();
}

void MainController::onLedConnected(const QString &deviceIp)
{
    // Если уже есть данный айпишник - то ничего не делаем, ибо все настроено
    if (m_ledControllersMap.contains(deviceIp)) return;

    m_ledControllersMap[deviceIp] = new LedController(deviceIp);

    createSubscription(deviceIp);

    connect(m_ledControllersMap[deviceIp], &LedController::dataReady, this, &MainController::onDataForLedReady);
    connect(m_ledControllersMap[deviceIp], &LedController::isOnline, this, &MainController::onLedOnlineStatus);

    onMqttMsgReceived(m_mqttClientHandler->getLastMsg(m_weatherTopic));
    onMqttMsgReceived(m_mqttClientHandler->getLastMsg(QString("%1%2/#").arg(m_taskTopicPrefix, deviceIp)));

    publishDiagnosticInfo(deviceIp, true);
}

void MainController::onLedResponse(const QString &deviceIP, const QByteArray& response)
{
    using namespace ISKRA;

    Packet in(response);

    qDebug() << QString("[%1][%2]: Ответ на команду %3 - %4")
                .arg(QDateTime::currentDateTime().time().toString(), deviceIP, toString(in.getCommand()), toString(in.getCommandResult()));

    if (m_ledControllersMap.contains(deviceIP))
        m_ledControllersMap.value(deviceIP)->onLedResponse();
}

void MainController::onDataForLedReady(const QString &deviceIP, QList<QByteArray> data, CurrentTableInfo info, bool isWeather)
{
    if (!isWeather) publishCurrentTableShowInfo(deviceIP, info);

    m_server->sendData(deviceIP, data);
}

void MainController::onLedOnlineStatus(const QString &deviceIP, bool isOnline)
{
    if (isOnline == false && m_ledControllersMap.contains(deviceIP))
    {
        disconnect(m_ledControllersMap[deviceIP], nullptr, nullptr, nullptr);
        m_ledControllersMap.remove(deviceIP);
        m_server->closeSocket(deviceIP);
    }

    publishDiagnosticInfo(deviceIP, isOnline);
}

void MainController::onMqttMsgReceived(QMqttMessage msg)
{
    const QString topicName = msg.topic().name();

    if (topicName.contains(m_taskTopicPrefix))
    {
        QRegExp rx("(?:[0-9]{1,3}\.){3}[0-9]{1,3}");

        int pos = rx.indexIn(topicName);

        if (pos != -1)
        {
            QStringList list = rx.capturedTexts();
            QString deviceIP = list.first();
            if (m_ledControllersMap.contains(deviceIP)) m_ledControllersMap[deviceIP]->onMqttMessageReceived(msg);
        }
        else
        {
            qWarning() << "Не получилось найти ip адресс в топике: " << topicName;
        }
    }

    if (topicName.contains(m_weatherTopic))
    {
        WeatherTopicParser topic(msg.payload());
        WeatherInfo info = topic.getData();

        if (topic.isValid())
        {
            // TODO: если с сервера будет слаться часто и одно и тоже, то есделать проверку на
            // температуру
            for (auto it = m_ledControllersMap.begin(); it != m_ledControllersMap.end(); ++it)
                it.value()->updateWeather(info);
        }
    }

    if (topicName.contains(m_emergencyTopic))
    {
        if (topicName == m_emergencyTopic)
        {
            for (auto it = m_ledControllersMap.begin(); it != m_ledControllersMap.end(); ++it)
                it.value()->onMqttMessageReceived(msg);
        }
        else
        {
            QString deviceIp = topicName.split("/").last();
            if (m_ledControllersMap.contains(deviceIp))
                m_ledControllersMap.value(deviceIp)->onMqttMessageReceived(msg);
        }
    }
}

void MainController::publishDiagnosticInfo(const QString &ip, bool isOnline)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime.setTimeSpec(Qt::UTC);
    dateTime.setTimeZone(QTimeZone::systemTimeZone());

    LedDiagnosticTopic topic(ip, isOnline, dateTime.toString(Qt::ISODate));

    if (!QMetaObject::invokeMethod(m_mqttClientHandler, "publish", Q_ARG(QString, m_diagnosticTopic + ip),
                                   Q_ARG(QByteArray, topic.getData()), Q_ARG(bool, true)))
    {
        qWarning() << "Can't invoke publish method of mqttHandler";
    }
}

void MainController::publishCurrentTableShowInfo(const QString &ip, CurrentTableInfo info)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime.setTimeSpec(Qt::UTC);
    dateTime.setTimeZone(QTimeZone::systemTimeZone());

    info.downloadTime = dateTime.toString(Qt::ISODate);

    CurrentTableInfoTopic topic(info);

    if (!QMetaObject::invokeMethod(m_mqttClientHandler, "publish", Q_ARG(QString, m_currentTableInfoTopic + ip),
                                   Q_ARG(QByteArray, topic.getData()), Q_ARG(bool, true)))
    {
        qWarning() << "Can't invoke publish method of mqttHandler";
    }
}

void MainController::createSubscription(const QString &deviceIP)
{
    QMqttTopicFilter filter;
    filter.setFilter(QString("%1%2/#").arg(m_taskTopicPrefix, deviceIP));

    emit subscribe(filter);
}


void MainController::run()
{
    if (!QMetaObject::invokeMethod(m_mqttClientHandler, "subscribe",
                                   Q_ARG(QMqttTopicFilter, QMqttTopicFilter(m_weatherTopic))))
    {
        qDebug() << "Невозможно вызвать invokeMethod mqtt subscribe для топика" << m_weatherTopic;
    }

    // TODO: убрать звездочку от сюда
    if (!QMetaObject::invokeMethod(m_mqttClientHandler, "subscribe",
                                   Q_ARG(QMqttTopicFilter, QMqttTopicFilter(m_emergencyTopic + "#"))))
    {
        qDebug() << "Невозможно вызвать invokeMethod mqtt subscribe для топика" << m_emergencyTopic + "#";
    }

    m_server = IskraTcpServer::getInstance();

    connect(m_server, &IskraTcpServer::connectToDevice, this, &MainController::onLedConnected);
    connect(m_server, &IskraTcpServer::gotResponse, this, &MainController::onLedResponse);
    connect(m_server, &IskraTcpServer::ledDisconnected, this, [this](const QString& ip)
    {
        onLedOnlineStatus(ip, false);
    });
}


