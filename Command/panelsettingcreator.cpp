#include "panelsettingcreator.h"

PanelSettingCreator::PanelSettingCreator(const PanelSettings& settings)
    : m_serverPort(settings.serverPort),
      m_tableWidth(settings.tableWidth),
      m_tableHeight(settings.tableHeight)
{
    reserve();

    m_ip = stringToVector(settings.ip);
    m_mask = stringToVector(settings.mask);
    m_gateway = stringToVector(settings.gateway);
    m_serverIp = stringToVector(settings.serverIp);
}

QByteArray PanelSettingCreator::getData()
{
    QByteArray data;

    data.append(vectorData(m_ip));
    data.append(vectorData(m_mask));
    data.append(vectorData(m_gateway));
    data.append(vectorData(m_serverIp));
    data.append(static_cast<char>(m_serverPort));
    data.append(static_cast<char>(m_tableWidth));
    data.append(static_cast<char>(m_tableHeight));

    return data;
}

void PanelSettingCreator::reserve()
{
    m_ip.reserve(4);
    m_mask.reserve(4);
    m_gateway.reserve(4);
    m_serverIp.reserve(4);
}

QByteArray PanelSettingCreator::vectorData(const QVector<uint8_t> &vector)
{
    QByteArray data;

    for (const auto& value : vector)
        data.append(static_cast<char>(value));

    return data;
}

QVector<uint8_t> PanelSettingCreator::stringToVector(const QString &str)
{
    QVector<uint8_t> result;
    result.reserve(4);

    QStringList splitStr = str.split(".");

    for (const auto& value : splitStr)
        result.append(value.toUInt());

    return result;
}
