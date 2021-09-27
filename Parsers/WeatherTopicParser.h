#pragma once

#include <QByteArray>

#include "Global.h"

using namespace Global::Defines;

class WeatherTopicParser
{
public:
    explicit WeatherTopicParser(const QByteArray& payload);
    inline WeatherInfo getData(){return m_info;}
    inline bool isValid(){return m_isValid;}
private:
    void parseData(const QByteArray& payload);
private:
    bool        m_isValid;
    WeatherInfo m_info;
};

