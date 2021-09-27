#pragma once

#include <QByteArray>
#include <QStringList>

#include <iterator>

class DateRepresentationTextCreator
{
public:
    explicit DateRepresentationTextCreator();
    QByteArray  getData();
    void appendDateTable(const QStringList& table);
private:
    QByteArray convertToByteArray(QStringList listTable);
    QByteArray buildData();
private:
    QList<QStringList> m_tabelsRepresentation;
};


