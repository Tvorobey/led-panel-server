#include "EmergencyTopicParser.h"

#include <QJsonObject>
#include <QJsonDocument>

namespace
{
    const QString FIELD_TITLE       = "title";
    const QString FIELD_SHOW_TIME   = "showTime";
    const QString FIELD_TEXT        = "text";
    const QString FIELD_REPEAT      = "repeat";
    const QString FIELD_CANCEL      = "cancel";
}

EmergencyTopicParser::EmergencyTopicParser(const QByteArray &payload)
    : m_isValid(false)
{
    parse(payload);
}

void EmergencyTopicParser::parse(const QByteArray &payload)
{
    QJsonObject obj = QJsonDocument::fromJson(payload).object();

    if (obj.contains(FIELD_TITLE) && obj.contains(FIELD_SHOW_TIME) && obj.contains(FIELD_TEXT) &&
        obj.contains(FIELD_CANCEL))
    {
        m_info.cancel = obj.value(FIELD_CANCEL).toBool();
        m_info.setShowTime(static_cast<uint32_t>(obj.value(FIELD_SHOW_TIME).toInt()));
        m_info.text = obj.value(FIELD_TEXT).toString();
        m_info.title = obj.value(FIELD_TITLE).toString();

        m_isValid = true;
    }
    else
        m_isValid = false;
}
