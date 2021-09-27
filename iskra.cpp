#include "iskra.h"

#include <QtDebug>
#include <QDataStream>

using namespace ISKRA;

/*
 *  class Packet
 */

const uint8_t    Packet::PckgSep1        = 0x5E;
const uint8_t    Packet::PckgSep2        = 0x3F;
const uint8_t    Packet::PckgPrefixEnd   = 0xFE;
const uint8_t    Packet::PckgSuffixEnd   = 0xFF;
const uint8_t    Packet::PckgDummyMiddle = 0x00;
const uint8_t    Packet::PckgDummyEnd    = 0x01;
const QByteArray Packet::PckgPrefix      = makePrefix();
const QByteArray Packet::PckgSuffix      = makeSuffix();


QByteArray Packet::makePrefix()
{
    return QByteArray().append(PckgSep1).append(PckgSep2).append(PckgPrefixEnd);
}

QByteArray Packet::makeSuffix()
{
    return QByteArray().append(PckgSep1).append(PckgSep2).append(PckgSuffixEnd);
}


QByteArray Packet::encode(const QByteArray& data)
{
    QByteArray packet((const char*)PckgPrefix,3);
    for(int i = 0; i < data.size(); i++)
    {
        packet.append(data[i]);
        if ( PckgSep1 == data[i] )
        {
            if ( i == data.size() - 1 )
                packet.append(PckgSep2).append(PckgDummyEnd);
            else if ( PckgSep2 == data[i+1] )
                packet.append(data[++i]).append(PckgDummyMiddle);
        }
    }
    packet.append((const char*)PckgSuffix,3);
    return packet;
}

uint32_t Packet::crc32(const QByteArray& data)
{
    uint32_t crc = 0xFFFFFFFF;
    for(uint8_t byte : data)
    {
        crc = crc ^ (byte << 24);
        for (int i = 0; i < 8; i++)
            crc = ((crc & 0x80000000) > 0) ? (crc << 1) ^ 0x04C11DB7 : crc << 1;
        crc = (uint32_t)(crc & 0xFFFFFFFF);
    }
    return crc;

}

QByteArray Packet::encode() const
{
    QByteArray packet;
    uint32_t dataSize = m_Data.size();
    packet.append(m_Receiver).append(m_Sender).append((uint8_t)m_Command).append((const char*)&dataSize,4).append(m_Data);
    uint32_t crc = crc32(packet);
    packet.append((const char*)&crc,4);

    return encode(packet);
}

void Packet::decode(const QByteArray &packet)
{
    if ( packet.startsWith(PckgPrefix) && packet.endsWith(PckgSuffix) )
    {
        QByteArray decoded;
        for(int i = 3; i < packet.size() - 3; i++)
        {
            decoded.append(packet[i]);
            if ( PckgSep1 == packet[i] && PckgSep2 == packet[i+1] && PckgDummyMiddle == packet[i+2] ) // подмена в середине
            {
                decoded.append(PckgSep2);
                i += 2; // съезжаем дальше
            }
            else if ( PckgSep1 == packet[i] && PckgSep2 == packet[i+1] && PckgDummyMiddle == packet[i+2] ) // подмена в конце
            {
                i += 2;
            }
            else if ( PckgSep1 == packet[i] && PckgSep2 == packet[i+1] ) // какая-то херня!!!!
            {
                qDebug() << "ГОВНО ПАКЕТ!!!!!";
                return;
            }
        }
        uint32_t crc = 0xFFFFFFFF;
        memcpy(&crc,(const char*)decoded.right(4),4);
        decoded.remove(decoded.size()-4,4);
        if ( crc != crc32(decoded) )
        {
            qDebug() << "Говённое CRC!!!!!";
            return;
        }
        m_Receiver = decoded[0];
        m_Sender   = decoded[1];

        m_Command  = Command((uint8_t)decoded[2]);
        decoded.remove(0,3);
        uint32_t length = 0;
        memcpy(&length,(const char*)decoded.left(4),4);
        decoded.remove(0,4);
        // TODO: uncomment if problem
//        qDebug() << "data length " << length;
        if ( length != (uint32_t)decoded.size() )
        {
            qDebug() << "Говённый размер!!!!!";
            return;
        }

        // Команда GetFirmwareInfoResponce (0х80) приходит в двух случаях:
        // 1. Мы запросили информацию о прошивке
        // 2. Мы решили сделать что то иное, и все пошло по п**де.
        // В этом случае, вместо заявленной команды ответа, приходит 0х80.
        // Отсюда и такое поведение, убираем первый бит только, когда приходит 0х80
        // В остальных случаях это портит пакет данных команд ответов
        if (m_Command == Command::GetFirmwareInfoResponce)
        {
            m_CommandResult = CommandResult((uint8_t)decoded[0]);
            decoded.remove(0,1);
        }
        else
            m_CommandResult = CommandResult::OK;


        m_Data = decoded;
    }
}
