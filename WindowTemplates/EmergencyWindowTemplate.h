#pragma once

#include "AbstractWindowTemplate.h"
#include "Global.h"

using namespace Global::Defines;
using namespace Global::Colors;
using namespace Global::Fonts;

class EmergencyWindowTemplate : public AbstractWindowTemplate
{
public:
    explicit EmergencyWindowTemplate(quint8 ledAddress, QObject* parent = nullptr);
    QList<QByteArray> getEmergencyInfo(int windowNum, const EmergencyInfo &info);
    QList<QByteArray> buildData() override {return QList<QByteArray>();}
    void reset() override {}
private:
    QList<QByteArray> buildEmergencyTemplate(int windowNum, const EmergencyInfo &info);
};

