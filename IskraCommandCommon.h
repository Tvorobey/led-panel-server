#pragma once

//Qt
#include <QString>
#include <QStringList>

namespace IskraCommon
{
    struct PanelSettings
    {
        PanelSettings()
            : ip("192.168.0.170"),
              mask("255.255.255.0"),
              gateway("192.168.0.1"),
              serverIp("192.168.0.17"),
              serverPort(quint8(3333)),
              tableWidth(quint8(256)),
              tableHeight(quint8(128))
        {}

        QString ip;
        QString mask;
        QString gateway;
        QString serverIp;
        quint8  serverPort;
        quint8  tableWidth;
        quint8  tableHeight;
    };

    struct RouteInfo
    {
        QString     routeNumber;
        QString     startStationName;
        QString     endStationName;
        QStringList stationsList;
    };
}
