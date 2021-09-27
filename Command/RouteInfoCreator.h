#pragma once

//Qt
#include <QObject>
/*!
 * \brief The RouteInfoCreator - класс, отвечающий за создание маршрутной информации
 */

enum Language
{
    Zero = 0,
    First,
    Second,
    UndefineLanguage
};

enum RouteSign
{
    Linear = 0,
    Circle,
    UndefineRoute
};

class RouteInfoCreator : public QObject
{
    Q_OBJECT
public:
    explicit RouteInfoCreator(QObject* parent = nullptr);
    void setLanguageAndRouteSign(const Language language, const RouteSign routeSign);
    void setRouteNumber(const QString& routeNumber);
    void setStartStationName(const QString& startStationName);
    void setEndStationName(const QString& endStationName);
    void setStationsList(const QStringList& stationsList);
    QByteArray getData() const;
private:
    QByteArray buildData() const;
    QByteArray encodeText(const QString& text) const;
private:
    Language     m_language;
    RouteSign    m_routeSign;
    QString      m_routeNumber;
    QString      m_startStationName;
    QString      m_endStationName;
    QStringList  m_stationsList;
};




