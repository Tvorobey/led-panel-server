//Local
#include "RouteInfoCreator.h"

//Qt
#include <QTextCodec>
#include <QDebug>


RouteInfoCreator::RouteInfoCreator(QObject* parent)
    : QObject(parent),
      m_language(UndefineLanguage),
      m_routeSign(UndefineRoute)
{}

void RouteInfoCreator::setLanguageAndRouteSign(const Language language,
                                               const RouteSign routeSign)
{
    m_language  = language;
    m_routeSign = routeSign;
}

void RouteInfoCreator::setRouteNumber(const QString& routeNumber)
{
    m_routeNumber = routeNumber;
}

void RouteInfoCreator::setStartStationName(const QString& startStationName)
{
    m_startStationName = startStationName;
}

void RouteInfoCreator::setEndStationName(const QString& endStationName)
{
    m_endStationName = endStationName;
}

void RouteInfoCreator::setStationsList(const QStringList& stationsList)
{
    m_stationsList = stationsList;
}

QByteArray RouteInfoCreator::getData() const
{
    return buildData();
}

QByteArray RouteInfoCreator::buildData() const
{
    QByteArray data;

    data.append(encodeText(QString::number(m_language) + QString::number(m_routeSign)));
    data.append(encodeText(m_routeNumber));
    data.append(encodeText(m_startStationName));
    data.append(encodeText(m_endStationName));
    for (const auto& stationName : m_stationsList)
        data.append(encodeText(stationName));

    return data;
}

QByteArray RouteInfoCreator::encodeText(const QString &text) const
{
    QTextCodec* codec = QTextCodec::codecForName("UTF16-LE");

    QByteArray data;

    data.append(codec->fromUnicode(text).remove(0, 2));
    data.append(static_cast<char>(0x0A));
    data.append(static_cast<char>(0x00));

    return data;
}


