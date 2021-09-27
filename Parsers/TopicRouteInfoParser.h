#pragma once

#include <QByteArray>
#include <QString>
#include <QList>

#include "Global.h"

using namespace Global::Defines;

class TopicRouteInfoParser
{
public:
    explicit TopicRouteInfoParser(const QByteArray& payload);
    StationInfo getStationInfo() {return m_info;}
    bool isValid() {return m_isValid;}
private:
    void parse(const QByteArray& payload);
private:
    bool m_isValid;
    StationInfo m_info;
};

