#pragma once

#include "Global.h"

using namespace Global::Defines;

class EmergencyTopicParser
{
public:
    explicit EmergencyTopicParser(const QByteArray& payload);
    inline EmergencyInfo getEmergencyInfo() {return m_info;}
    inline bool isValid() {return m_isValid;}
private:
    void parse(const QByteArray& payload);
private:
    bool            m_isValid;
    EmergencyInfo   m_info;
};

