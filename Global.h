#pragma once
#include <QMap>

#include "WindowCreator.h"

namespace Global
{
    namespace PanelInformation
    {
        enum Orientation
        {
            Horizontal = 0,
            VerticalCounterClockWise,
            VerticalClockWise,
            HorizontalReverse
        };

        const QMap<Orientation, int> ORIENTATION_TO_DEGREE
        {
            {Horizontal,                0},
            {VerticalCounterClockWise,  -90},
            {VerticalClockWise,         90},
            {HorizontalReverse,         180}
        };

        enum PanelType
        {
            Small = 0,
            Big
        };
    }

    namespace Defines
    {
        struct VehicleInfo
        {
            uint32_t    id = 0;
            QString     routeNum = "";
            QString     endStation = "";
            uint32_t    timeToArrive = 0;

            friend bool operator== (const VehicleInfo& lhs, const VehicleInfo& rhs)
            {
                bool result = false;

                if ((lhs.id == rhs.id) && (lhs.routeNum == rhs.routeNum)  && (lhs.endStation == rhs.endStation) && (lhs.timeToArrive && rhs.timeToArrive))
                    result = true;

                return result;
            }            
        };

        struct StationInfo
        {
            QString stationName = "";
            QList<VehicleInfo>  arrivnigVehicles;
        };

        struct WeatherInfo
        {
            int currentTemp = 0;
        };

        struct EmergencyInfo
        {
            QString title = "";
            void setShowTime(u_int32_t time){showTime = time * 16;}
            uint32_t showTime = 0; // Если 0 - информация выводится бесконечно, до отмены
            QString text = "";
            bool cancel = false; // Если true, то убираем вывод данной информации
        };

        struct CurrentTableInfo
        {
            QString downloadTime = "";
            QString stationName = "";
            QStringList imageLibrary = {};
            QList<VehicleInfo> arrivingVehicles = {};
        };
    }

    namespace Colors
    {
        const Window::Color WHITE   = Window::Color(255, 255, 255, 255);
        const Window::Color BLUE    = Window::Color(10, 62, 0, 255);
        const Window::Color BLACK   = Window::Color(255, 0, 0, 0);
        const Window::Color GREEN   = Window::Color(255, 1, 254, 0);
        const Window::Color PIRPLE  = Window::Color(255, 213, 27, 229);
        const Window::Color RED     = Window::Color(255, 255, 0, 0);
        const Window::Color TRANSPARENT = Window::Color(0, 0, 0, 0);
    }

    namespace WeatherColorRange
    {
        const Window::Color BLUE            = Window::Color(255, 28, 0, 255);
        const Window::Color GREEN           = Window::Color(255, 0, 255, 18);
        const Window::Color YELLOW          = Window::Color(255, 250, 255, 0);
        const Window::Color RED             = Window::Color(255, 255, 69, 0);

        const QMap<QPair<int, int>, Window::Color> TEMPERATURE_COLOR_MAP
        {
            {{-60, 2}, BLUE},
            {{3, 20}, GREEN},
            {{21, 30}, YELLOW},
            {{31, 70}, RED}
        };
    }

    namespace Fonts
    {
        constexpr int FONT_NUMBER = 0;
        constexpr int FONT_NUMBER_BIG = 1;
    }

    namespace Envirenment
    {
        const QString SERVER_ADDRESS            = "SERVER_ADDRESS";
        const QString SERVER_PORT               = "SERVER_PORT";
        const QString PANEL_ADDRESS             = "PANEL_ADDRESS";
        const QString KEEP_ALIVE                = "KEEP_ALIVE";
        const QString MQTT_HOST                 = "MQTT_HOST";
        const QString MQTT_PORT                 = "MQTT_PORT";
        const QString MQTT_KEEP_ALIVE           = "MQTT_KEEP_ALIVE";        
        const QString TASK_TOPIC                = "TASK_TOPIC";
        const QString TASK_TOPIC_POSTFIX        = "TASK_TOPIC_POSTFIX";
        const QString WEATHER_TOPIC             = "WEATHER_TOPIC";
        const QString EMERGENCY_TOPIC           = "EMERGENCY_TOPIC";
        const QString DIAGNOSTIC_TOPIC          = "DIAGNOSTIC_TOPIC";
        const QString SHOW_TEMPERATURE_DELAY    = "SHOW_TEMPERATURE_DELAY";
        const QString IMAGES_PATH               = "IMAGES_PATH";
        const QString LOGO_IMAGE_NAME           = "LOGO_IMAGE_NAME";
        const QString CURRENT_TABLE_INFO_TOPIC  = "CURRENT_TABLE_INFO_TOPIC";
    }
}
