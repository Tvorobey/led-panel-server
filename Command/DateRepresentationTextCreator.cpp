#include "DateRepresentationTextCreator.h"

#include <QStringList>
#include <QVector>
#include <QMap>
#include <QTextCodec>
#include <QDebug>
#include <QDataStream>

namespace
{
    const QString END_OF_STR = QString(char(0));
    const uint16_t FIRST_TABLE_VALUE_ADDR = 0x13;
    const uint16_t FIRST_TABLE_ADDR = 0x0C; // Без учета количества двубайтных слов
    const int NUMBER_OF_TABLE_NUMBER_BYTE = 4;

    uint32_t CRC32(const QByteArray& data)
    {
        uint32_t crc = 0xFFFFFFFF;
        for(uint8_t byte : data)
        {
            crc = crc ^ (byte << 24);
            for (int i = 0; i < 8; i++)
                crc = ((crc & 0x80000000) > 0) ? (crc << 1) ^ 0x04C11DB7 : crc << 1;
            crc = uint32_t(crc & 0xFFFFFFFF);
        }
        return crc;
    }
}

DateRepresentationTextCreator::DateRepresentationTextCreator()
{}

QByteArray DateRepresentationTextCreator::getData()
{
    return buildData();
}

void DateRepresentationTextCreator::appendDateTable(const QStringList &table)
{
    m_tabelsRepresentation.append(table);
}

QByteArray DateRepresentationTextCreator::convertToByteArray(QStringList listTable)
{
    // Добавили в каждое слово символ окончания
    for (auto& str : listTable)
        str.append(END_OF_STR);

    // Собираем массив с адрессами названий в таблице
    QVector<uint16_t> valuesAddresses;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (int i = 0; i < listTable.size(); ++i)
    {
        if (!listTable[i].isEmpty())
        {
            if (i == 0)
                valuesAddresses.append(FIRST_TABLE_VALUE_ADDR);
            else
                valuesAddresses.append(valuesAddresses[i - 1] + listTable[i - 1].size());

            stream << valuesAddresses[i];
        }
    }

    QTextCodec* codec = QTextCodec::codecForName("UTF-16LE");

    for (const auto& str : listTable)
        data.append(codec->fromUnicode(str).remove(0,2));

    return data;
}

QByteArray DateRepresentationTextCreator::buildData()
{
    QByteArray data;
    QByteArray byteCRC;
    QList<QByteArray>   binaryTablesList;
    QVector<uint16_t>   tableAdresses;
    uint32_t            summaryTableSize(0);
    uint32_t            summaryTableCRC(0);
    uint32_t            numberOfTables(0);

    // Собрали таблицы в бинарный вид
    for (QStringList table : m_tabelsRepresentation)
        binaryTablesList.append(convertToByteArray(table));

    numberOfTables = static_cast<uint32_t>(binaryTablesList.size());

    for (int i = 0; i < binaryTablesList.size(); ++i)
    {
        if (i == 0)
            // Адрес таблицы от начала пакета, тут еще учитываем, что у нас N
            // двухбайтных слов с адресами таблиц. Отсюда и binaryTablesList.size()*2
            tableAdresses.append(FIRST_TABLE_ADDR + binaryTablesList.size()*2);
        else
        {
            int summarySizeOfPreviousTable(0);

            for (int j = 0; j < i; ++j)
                summarySizeOfPreviousTable += binaryTablesList[j].size();

            tableAdresses.append(FIRST_TABLE_ADDR + binaryTablesList.size()*2
                                 + summarySizeOfPreviousTable);
        }
    }

    // Считаем суммарный размер таблиц, начиная с 4 байт количества таблиц
    summaryTableSize += NUMBER_OF_TABLE_NUMBER_BYTE; // Количество таблиц
    summaryTableSize += static_cast<uint32_t>(tableAdresses.size()*2);  // N двухбайтных слов
                                                                        // Адреса таблиц
    for (const auto& table : binaryTablesList)
        summaryTableSize += static_cast<uint32_t>(table.size());    // Размеры таблиц

    // Собираем QByteArray для дальнейшего расчета CRC
    QDataStream streamCRC(&byteCRC, QIODevice::WriteOnly | QIODevice::Append);
    streamCRC.setByteOrder(QDataStream::LittleEndian);

    streamCRC << numberOfTables;
    for (const auto& addr : tableAdresses)
        streamCRC << addr;
    for (const auto& binaryTable : binaryTablesList)
        byteCRC.append(binaryTable);

    // Считаем CRC
    summaryTableCRC = CRC32(byteCRC);

    // Собираем итоговый массив данных
    QDataStream streamData(&data, (QIODevice::WriteOnly | QIODevice::Append));
    streamData.setByteOrder(QDataStream::LittleEndian);

    streamData << summaryTableSize;
    streamData << summaryTableCRC;
    streamData << numberOfTables;

    for (const auto& addr : tableAdresses)
        streamData << addr;

    for (const auto& binaryTable : binaryTablesList)
        data.append(binaryTable);

    return data;
}
