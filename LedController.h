#pragma once

#include <QObject>
#include <QMqttSubscription>
#include <QTimer>
#include <QSemaphore>

#include "Global.h"
#include "IskraSimpleCommand.h"
#include "RouteInfoTemplate.h"

using namespace Global::Defines;
using namespace Global::Envirenment;
using namespace Global::WeatherColorRange;

class LedController : public QObject
{
    Q_OBJECT
public:
    explicit LedController(const QString& ip, QObject *parent = nullptr);
    ~LedController() override;
signals:
    /*!
     * \brief dataReady - сигнализирует о том, что данные для табло готовы
     * \param data - сигнализирует о том, что за время n не было полученно сообщения от табло,
     * следовательно, табло отвалилось
     */

    //TODO: продумать, как лучше это сделать, хуета какая то вышла, потому что надо было быстро
    void dataReady(const QString& ip, QList<QByteArray> data, CurrentTableInfo info, bool isWeather);
    void isOnline(const QString& ip, bool isOnline);
public slots:
    void onLedResponse();
    void onMqttMessageReceived(QMqttMessage msg);
    void updateWeather(const WeatherInfo& weatherInfo);
private:
    void onBuildRouteInfoTopic(const QByteArray& payload);
    void onEmergencyTopic(const QByteArray& payload);
private:
    QString                             m_ip;
    QTimer                              m_keepAliveTimer;
    // NOTE: средствами табло не получилось нормально сделать....
    QTimer                              m_showTempTimer;
    QTimer                              m_sendDiagnosticTimer;
    StationInfo                         m_stationInfo;
    QScopedPointer<IskraSimpleCommand>  m_simpleCommand;
    QScopedPointer<RouteInfoTemplate>   m_windowCreator;
    int                                 m_ledAddress;
    QString                             m_stationInfoTopic;
    QStringList                         m_images;
    bool                                m_showTemp;
    QSemaphore                          m_semaphore;
    int                                 m_pingRetry;
    bool                                m_isOnline;
};

