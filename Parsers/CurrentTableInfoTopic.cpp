#include "CurrentTableInfoTopic.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

namespace
{
    const QString DOWNLOAD_TIME_FIELD = "downloadTime";
    const QString STATION_NAME_FIELD = "stationName";
    const QString IMAGE_LIBRARY_FIELD = "imageLibrary";
    const QString FIELD_ARRIVING_VEHILCLES  = "arrivingVehicle";
    const QString FIELD_ID                  = "id";
    const QString FIELD_ROUTE_NUM           = "routeNum";
    const QString FIELD_END_STATION         = "endStation";
    const QString FIELD_TIME_TO_ARRIVE      = "timeToArrive"; // Время прибытия в секундах
}

CurrentTableInfoTopic::CurrentTableInfoTopic(const CurrentTableInfo &info)
{
    buildData(info);
}

void CurrentTableInfoTopic::buildData(const CurrentTableInfo &info)
{
    QJsonObject obj;

    obj.insert(DOWNLOAD_TIME_FIELD, info.downloadTime);
    obj.insert(STATION_NAME_FIELD, info.stationName);

    QJsonArray imageLibraryArr;
    for (const auto& image : info.imageLibrary)
        imageLibraryArr.append(image);
    obj.insert(IMAGE_LIBRARY_FIELD, imageLibraryArr);

    QJsonArray arrivingVehicleArr;
    for (const auto& vehicle : info.arrivingVehicles)
    {
        QJsonObject vehicleObj;
        vehicleObj.insert(FIELD_ID, QString::number(vehicle.id));
        vehicleObj.insert(FIELD_ROUTE_NUM, vehicle.routeNum);
        vehicleObj.insert(FIELD_END_STATION, vehicle.endStation);
        vehicleObj.insert(FIELD_TIME_TO_ARRIVE, QString::number(vehicle.timeToArrive));

        arrivingVehicleArr.append(vehicleObj);
    }

    obj.insert(FIELD_ARRIVING_VEHILCLES, arrivingVehicleArr);

    m_payload = QJsonDocument(obj).toJson();
}
