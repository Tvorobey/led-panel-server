#include "TopicRouteInfoParser.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

namespace
{
    const QString FIELD_STATION_NAME        = "stationName";
    const QString FIELD_ARRIVING_VEHILCLES  = "arrivingVehicle";
    const QString FIELD_ID                  = "id";
    const QString FIELD_ROUTE_NUM           = "routeNum";
    const QString FIELD_END_STATION         = "endStation";
    const QString FIELD_TIME_TO_ARRIVE      = "timeToArrive"; // Время прибытия в секундах

    auto secToMin = [](int secs)
    {
        return qRound(static_cast<double>(secs/60));
    };
}

TopicRouteInfoParser::TopicRouteInfoParser(const QByteArray &payload)
{
    parse(payload);
}

void TopicRouteInfoParser::parse(const QByteArray &payload)
{
    QJsonObject obj = QJsonDocument::fromJson(payload).object();

    if (obj.isEmpty())
    {
        qWarning() << "Got empty ROUTE_INFO";
        m_isValid = false;
        return;
    }

    if (obj.contains(FIELD_STATION_NAME) && obj.contains(FIELD_ARRIVING_VEHILCLES))
    {
        m_info.stationName = obj.value(FIELD_STATION_NAME).toString();
        QJsonArray arrivingVehicles = obj.value(FIELD_ARRIVING_VEHILCLES).toArray();

        for (auto it = arrivingVehicles.begin(); it != arrivingVehicles.end(); ++it)
        {
            QJsonObject vehicleObj = it->toObject();

            VehicleInfo info;
            info.id = static_cast<uint32_t>(vehicleObj.value(FIELD_ID).toInt());
            info.routeNum = vehicleObj.value(FIELD_ROUTE_NUM).toString();
            info.endStation = vehicleObj.value(FIELD_END_STATION).toString();
            info.timeToArrive = static_cast<uint32_t>(secToMin(vehicleObj.value(FIELD_TIME_TO_ARRIVE).toInt()));

            m_info.arrivnigVehicles.append(info);
        }

        m_isValid = true;
    }
    else
    {
        m_isValid = false;
        return;
    }
}


