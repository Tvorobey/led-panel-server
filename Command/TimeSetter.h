#ifndef TIMESETTER_H
#define TIMESETTER_H

//Qt
#include <QByteArray>

//TODO: класс, похожий на тот который можно вынести в класс с простыми коммандами

class TimeSetter
{
public:
    explicit TimeSetter();
    static QByteArray getData();
};

#endif // TIMESETTER_H
