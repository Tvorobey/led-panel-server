#pragma once

#include <QObject>

enum State
{
    Motion = 0,
    Station
};

class RoutePositionCreator : public QObject
{
    Q_OBJECT
public:
    explicit RoutePositionCreator(QObject *parent = nullptr);
    static QByteArray getData(const qint8 currentStationIndex, const State state);
};


