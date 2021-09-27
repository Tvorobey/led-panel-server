#include "WeatherTopicParser.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

namespace
{
    const QString& FIELD_CURRENT_TEMP = "currentTemp";
}

WeatherTopicParser::WeatherTopicParser(const QByteArray &payload)
    : m_isValid(false)
{
    parseData(payload);
}

void WeatherTopicParser::parseData(const QByteArray &payload)
{
    QJsonObject obj = QJsonDocument::fromJson(payload).object();

    if (obj.isEmpty())
    {
        qWarning() << "Получили пустой топик WEATHER";
        return;
    }

    if (obj.contains(FIELD_CURRENT_TEMP))
    {
        m_info.currentTemp = obj.value(FIELD_CURRENT_TEMP).toInt();
        m_isValid = true;
        return;
    }

    qDebug() << "Неверный формат топика WEATHER";
}
