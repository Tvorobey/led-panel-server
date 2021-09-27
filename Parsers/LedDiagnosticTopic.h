#pragma once

#include <QString>
#include <QJsonDocument>

class LedDiagnosticTopic
{
public:
    explicit LedDiagnosticTopic(const QString& ip, bool isOnline, const QString &lastTimeOnline);
    inline QByteArray getData() {return m_payload;}
private:
    void buildData(const QString& ip, bool isOnline, const QString &lastTimeOnline);
private:
    QByteArray  m_payload;
};

