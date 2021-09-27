//Local
#include "TimeSetter.h"
//Qt
#include <QDateTime>

TimeSetter::TimeSetter()
{}

QByteArray TimeSetter::getData()
{
    QByteArray data;

    QTime time(QTime::currentTime());
    QDate date(QDate::currentDate());

    data.append(time.hour());
    data.append(time.minute());
    data.append(time.second());
    data.append(date.dayOfWeek());
    data.append(date.day());
    data.append(date.month());
    data.append(date.year());

    return data;
}
