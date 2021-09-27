//Local
#include "RoutePositionCreator.h"

//Qt
#include <QDataStream>

RoutePositionCreator::RoutePositionCreator(QObject *parent)
    : QObject(parent)
{}

QByteArray RoutePositionCreator::getData(const qint8 currentStationIndex, const State state)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << currentStationIndex << static_cast<qint8>(state);

    return data;
}

