#pragma once

#include <QByteArray>

#include "Global.h"

using namespace Global::Defines;

class CurrentTableInfoTopic
{
public:
    explicit CurrentTableInfoTopic(const CurrentTableInfo& info);
    inline QByteArray getData() {return m_payload;}
    void buildData(const CurrentTableInfo& info);
private:
    QByteArray  m_payload;
};

