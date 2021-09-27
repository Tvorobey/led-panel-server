#include "LedController.h"
#include "Parsers/TopicRouteInfoParser.h"
#include "Parsers/EmergencyTopicParser.h"
#include "Parsers/CurrentTableInfoTopic.h"
#include "EmergencyWindowTemplate.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QProcessEnvironment>
#include <numeric>

namespace
{
    enum TopicTask
    {
        buildRouteInfo = 0,
    };

    /*Из скольки частей состоит строка информации, относящаяся к одному логическому элементу*/
    constexpr int ADDITIOANL_INFO_WINDOW_SIZE = 3;
    constexpr int ARRIVING_VEHICLE_WINDOW_SIZE = 3;
    constexpr int EMERGENCY_WINDOW_SIZE = 2;
    constexpr int LOCAL_KEEP_ALIVE = 5000;
    constexpr int KEEP_ALIVE_RETRY = 2; // Количество повторов, при истечении таймера ответов

    /*!
     * \brief The WindowsNum - нумерация окон для создания.
     * Стоит отметить то, что firstAdditionalWindow - указывает на то, что в данной линии будет езе
     * несколько окон
     */
    enum WindowsNum
    {
        bgnWindow = 0,
        stationNameWindow = 1,
        firstAdditionalInfoWindow = 2,
        temperatureWindow = firstAdditionalInfoWindow + ADDITIOANL_INFO_WINDOW_SIZE,
        firstArrivingVehicleWindow = temperatureWindow + 1,
        emergencyWindow = 20
    };

    QMap<QString, TopicTask> TOPIC_POSTFIX {};

    int KEEP_ALIVE_DELAY;
    int TEMP_TIMER_DELAY;    
    int CURRENT_TEMP;

    Window::Color CURRENT_TEMP_COLOR(255, 255, 255, 255);

    auto getTempColor = [](int temp)
    {
        for (auto it = TEMPERATURE_COLOR_MAP.begin(); it != TEMPERATURE_COLOR_MAP.end(); ++it)
        {
            if ((temp >= it.key().first) && (temp <= it.key().second))
                return it.value();
        }

        return WHITE;
    };

    auto filterArrivingVehicles = [](const QList<VehicleInfo>& input)
    {
        QList<VehicleInfo> tmp;
        QMap<QPair<QString, QString>, QList<VehicleInfo>> map;

        for (const auto& item : input)
        {
            if (map.contains({item.routeNum, item.endStation})) map[{item.routeNum, item.endStation}].append(item);
            else map.insert({item.routeNum, item.endStation}, {item});
        }

        /*Собираем список ближайших ТС, которые к нам подъезжают*/
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            VehicleInfo nearestVehicle = *std::min_element(it.value().begin(), it.value().end(),
                                                           [](const VehicleInfo& lhs, const VehicleInfo& rhs)
            {
                return lhs.timeToArrive < rhs.timeToArrive;
            });

            tmp.append(nearestVehicle);
        }

        qSort(tmp.begin(), tmp.end(), [](const VehicleInfo& lhs, const VehicleInfo& rhs)
        {
            return lhs.timeToArrive < rhs.timeToArrive;
        });

        QList<VehicleInfo> result;

        for (int i = 0; i < 4; ++i)
        {
            if (i < tmp.size()) result.append(tmp[i]);
        }

        return result;
    };
}

LedController::LedController(const QString &ip, QObject *parent)
    : QObject(parent),
      m_ip(ip),
      m_showTemp(false),
      m_pingRetry(KEEP_ALIVE_RETRY),
      m_isOnline(true)
{
    QProcessEnvironment variable = QProcessEnvironment::systemEnvironment();
    m_ledAddress = variable.value(PANEL_ADDRESS).isEmpty() ? 0 : variable.value(PANEL_ADDRESS).toInt();
    m_stationInfoTopic = variable.value(TASK_TOPIC_POSTFIX).isEmpty() ? "ROUTE_INFO" : variable.value(TASK_TOPIC_POSTFIX);
    KEEP_ALIVE_DELAY = variable.value(KEEP_ALIVE).isEmpty() ? LOCAL_KEEP_ALIVE : variable.value(KEEP_ALIVE).toInt();
    TEMP_TIMER_DELAY = variable.value(SHOW_TEMPERATURE_DELAY).isEmpty() ? 5000 : variable.value(SHOW_TEMPERATURE_DELAY).toInt();


    TOPIC_POSTFIX.insert(m_stationInfoTopic, buildRouteInfo);

    m_simpleCommand.reset(new IskraSimpleCommand(0, this));
    m_windowCreator.reset(new RouteInfoTemplate(0, this));

    m_keepAliveTimer.start(KEEP_ALIVE_DELAY);

    connect(&m_keepAliveTimer, &QTimer::timeout, this, [this]()
    {
        if (m_pingRetry == 0)
        {
            // Ну теперь уж запингуем....
            int pingExitCode = QProcess::execute("ping", {"-c", "1", m_ip});

            if (pingExitCode != 1)
            {
                m_pingRetry = KEEP_ALIVE_RETRY;
                m_keepAliveTimer.start(KEEP_ALIVE_DELAY);
                return; // пингуется
            }

            m_keepAliveTimer.stop();
            m_showTempTimer.stop();
            disconnect(&m_keepAliveTimer, nullptr, nullptr, nullptr);
            disconnect(&m_showTempTimer, nullptr, nullptr, nullptr);

            qDebug() << QString("[%1]: Disconnected").arg(m_ip);

            m_isOnline = false;

            emit isOnline(m_ip, m_isOnline);
        }
        else
        {
            --m_pingRetry;            
            m_keepAliveTimer.start(KEEP_ALIVE_DELAY);
        }
    });

    QFuture<void> clearTask = QtConcurrent::run([=]()
    {
        QList<QByteArray> data = m_simpleCommand->packetDeleteAllWindow();

        // Думаю, что данная загрузка должна просто управляться отдельно
        if (!variable.value(LOGO_IMAGE_NAME).isEmpty())
        {
            QList<QVariant> images;
            m_images.append(variable.value(LOGO_IMAGE_NAME));
            images.append(variable.value(IMAGES_PATH) + variable.value(LOGO_IMAGE_NAME));
            data.append(m_simpleCommand->packetLoadImageLibrary(images));
        }
        else
        {
            m_images.append("TT_logo.png");
            QList<QVariant> images;
            images.append("/home/user/Pictures/TT_logo.png");
            data.append(m_simpleCommand->packetLoadImageLibrary(images));
        }

        CurrentTableInfo info;
        info.imageLibrary.append(variable.value(LOGO_IMAGE_NAME));

        emit dataReady(m_ip, data, info, false);

        m_semaphore.release(1);
    });
}

LedController::~LedController()
{
    qDebug() << QString("[%1] - Delete LedController").arg(m_ip);
}

void LedController::onLedResponse()
{
    if (m_isOnline != true)
    {
        m_isOnline = true;

        emit isOnline(m_ip, m_isOnline);
    }

    m_pingRetry = KEEP_ALIVE_RETRY;
    m_keepAliveTimer.start(KEEP_ALIVE_DELAY);
}

// TODO: не нравится этот момент - не универсальный, хотя...  если сделать подписку на топики от сюда
// ведь по сути, они нужны контроллеру табло, а не главному контроллеру
void LedController::onMqttMessageReceived(QMqttMessage msg)
{
    const QString topic = msg.topic().name();

    if (topic.contains("TASK"))
    {
        const QString command = msg.topic().name().split("/").last();

        switch (TOPIC_POSTFIX.value(command))
        {
            case buildRouteInfo:  onBuildRouteInfoTopic(msg.payload()); break;
        }
    }
    else if (topic.contains("EMERGENCY"))
    {
        onEmergencyTopic(msg.payload());
    }
}

void LedController::updateWeather(const WeatherInfo &weatherInfo)
{
    CURRENT_TEMP = weatherInfo.currentTemp;
    CURRENT_TEMP_COLOR = getTempColor(weatherInfo.currentTemp);

    if (m_showTempTimer.isActive()) return;

    connect(&m_showTempTimer, &QTimer::timeout, this, [this]()
    {
        if (m_stationInfo.stationName.isEmpty())
        {
            return;
        }

        QFuture<void> createData = QtConcurrent::run([=]()
        {
            QList<QByteArray> data;

            if (m_showTemp)
            {
                data.append(m_windowCreator->getAdditionalWindow(WindowsNum::firstAdditionalInfoWindow,
                                                                 m_showTemp, CURRENT_TEMP, CURRENT_TEMP_COLOR));
                m_showTemp = false;

                emit dataReady(m_ip, data, CurrentTableInfo(), true);
            }
            else
            {
                data.append(m_windowCreator->getAdditionalWindow(WindowsNum::firstAdditionalInfoWindow));

                m_showTemp = true;

                emit dataReady(m_ip, data, CurrentTableInfo(), true);
            }
        });
    });

    m_showTempTimer.start(TEMP_TIMER_DELAY);
}

void LedController::onBuildRouteInfoTopic(const QByteArray &payload)
{
    QFuture<void> prepareStationData = QtConcurrent::run([=]()
    {
        m_semaphore.acquire(1);

        TopicRouteInfoParser parser(payload);

        if (parser.isValid())
        {
            auto newStationInfo = parser.getStationInfo();
            /*Фильтруем ТС, которые к нам едут, оставляем только ближайшие*/
            newStationInfo.arrivnigVehicles = filterArrivingVehicles(newStationInfo.arrivnigVehicles);

            QList<QByteArray> data;

            if (m_stationInfo.stationName != newStationInfo.stationName)
            {
                m_stationInfo = newStationInfo;

                data.append(m_simpleCommand->packetDeleteAllWindow());
                data.append(m_simpleCommand->packetSetTime());
                data.append(m_windowCreator->getStationNameWindow(m_stationInfo.stationName, WindowsNum::stationNameWindow));
                data.append(m_windowCreator->getAdditionalWindow(WindowsNum::firstAdditionalInfoWindow));

                int numberOfCreatedWindow = 0;

                for (const auto& info : qAsConst(m_stationInfo.arrivnigVehicles))
                {
                    int firstWindowNum = WindowsNum::firstArrivingVehicleWindow + numberOfCreatedWindow * ARRIVING_VEHICLE_WINDOW_SIZE;
                    data.append(m_windowCreator->getArrivingVehicleWindow(info, firstWindowNum, numberOfCreatedWindow));
                    ++numberOfCreatedWindow;
                }


            }
            else
            {
                if (m_stationInfo.arrivnigVehicles.size() > newStationInfo.arrivnigVehicles.size())
                {
                    /*Количество строк, которые надо удалить*/
                    int delta = m_stationInfo.arrivnigVehicles.size() - newStationInfo.arrivnigVehicles.size();
                    /*Номер строки, с которой будем начинать удаление*/
                    int deleteFromString = m_stationInfo.arrivnigVehicles.size() - delta;
                    /*Номер окна с которого будем удалять*/
                    int deleteFromWindow = WindowsNum::firstArrivingVehicleWindow + ARRIVING_VEHICLE_WINDOW_SIZE * deleteFromString;

                    QList<qint8> windowNumForDelete;

                    for (int i = 0; i < delta * ARRIVING_VEHICLE_WINDOW_SIZE; ++i)
                        windowNumForDelete << deleteFromWindow + i;

                    data.append(m_simpleCommand->packetDeleteSelectWindow(windowNumForDelete));
                }

                m_stationInfo.arrivnigVehicles = newStationInfo.arrivnigVehicles;

                int numberOfCreatedWindow = 0;

                for (const auto& info : qAsConst(m_stationInfo.arrivnigVehicles))
                {
                    int firstWindowNum = WindowsNum::firstArrivingVehicleWindow + numberOfCreatedWindow * ARRIVING_VEHICLE_WINDOW_SIZE;
                    data.append(m_windowCreator->getArrivingVehicleWindow(info, firstWindowNum, numberOfCreatedWindow));
                    ++numberOfCreatedWindow;
                }                
            }

            CurrentTableInfo info;
            info.stationName = m_stationInfo.stationName;
            info.imageLibrary = m_images;
            info.arrivingVehicles = m_stationInfo.arrivnigVehicles;

            emit dataReady(m_ip, data, info, false);
        }
        else qDebug() << "Invalid ROUTE_INFO topic";

        m_semaphore.release(1);
    });
}

void LedController::onEmergencyTopic(const QByteArray &payload)
{
    QFuture<void> prepareEmergencyData = QtConcurrent::run([=]()
    {
        EmergencyTopicParser topic(payload);

        if (topic.isValid())
        {
            QList<QByteArray> data;

            EmergencyInfo info = topic.getEmergencyInfo();

            if (info.cancel == true) data.append(m_simpleCommand->packetDeleteSelectWindow({emergencyWindow, emergencyWindow + 1}));
            else
            {
                EmergencyWindowTemplate windowCreator(m_ledAddress);
                data.append(windowCreator.getEmergencyInfo(emergencyWindow, topic.getEmergencyInfo()));
            }

            // TODO: Ну вот это тоже не правильно, тут надо отправлять экстренную инфу - костыль
            emit dataReady(m_ip, data, CurrentTableInfo(), true);
        }
        else
        {
            qWarning() << "Invalid Emergency topic";
        }
    });
}
