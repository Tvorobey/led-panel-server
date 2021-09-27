#pragma once
//Local
#include "IskraCommandCommon.h"
//Qt
#include <QVector>
//STL
#include <cstdint>

using namespace IskraCommon;

class PanelSettingCreator
{
public:
    explicit PanelSettingCreator(const PanelSettings& settings);
    QByteArray getData();
private:
    void reserve();
    QByteArray vectorData(const QVector<uint8_t>& vector);
    QVector<uint8_t> stringToVector(const QString& str);
private:
    QVector<uint8_t>    m_ip;
    QVector<uint8_t>    m_mask;
    QVector<uint8_t>    m_gateway;
    QVector<uint8_t>    m_serverIp;
    // TODO: uint16_t
    uint8_t             m_serverPort;
    uint8_t             m_tableWidth;
    uint8_t             m_tableHeight;
};


