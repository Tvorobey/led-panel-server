#include "LedDiagnosticTopic.h"

#include <QJsonObject>

namespace
{
    const QString   FIELD_IP                = "ip";
    const QString   FIELD_ONLINE            = "online";
    const QString   FIELD_LAST_ONLINE_TIME  = "lastPingTime";
}

LedDiagnosticTopic::LedDiagnosticTopic(const QString &ip, bool isOnline, const QString &lastTimeOnline)
{
    buildData(ip, isOnline, lastTimeOnline);
}

void LedDiagnosticTopic::buildData(const QString &ip, bool isOnline, const QString &lastTimeOnline)
{
    QJsonObject obj;
    obj.insert(FIELD_IP, ip);
    obj.insert(FIELD_ONLINE, isOnline);
    obj.insert(FIELD_LAST_ONLINE_TIME, lastTimeOnline);

    m_payload = QJsonDocument(obj).toJson();
}
