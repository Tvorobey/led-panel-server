#include "BrightnessAdjustmentRableCreator.h"

#include <QVector>

typedef QPair<int,int> brightnessAndValue;

namespace
{
    const QVector<brightnessAndValue> BRIGHTNESS_TABLE
    {
        {0, 150},
        {16, 150},
        {32, 150},
        {48, 150},
        {64, 150},
        {80, 150},
        {96, 150},
        {112, 150},
        {128, 150},
        {144, 150},
        {160, 150},
        {176, 150},
        {192, 150},
        {208, 150},
        {224, 150},
        {240, 150},
        {256, 150}
    };

    const int SPEED = 8;
}

BrightnessAdjustmentTableCreator::BrightnessAdjustmentTableCreator()
{

}

void BrightnessAdjustmentTableCreator::setBrightnessAdjustmentTable(const QList<QPair<int, int> > table)
{
    Q_UNUSED(table)
}

QByteArray BrightnessAdjustmentTableCreator::buildData() const
{
    QByteArray data;

    for (const auto& value : BRIGHTNESS_TABLE)
        data.append(value.second);
    data.append(SPEED);

    return data;
}

QByteArray BrightnessAdjustmentTableCreator::getData() const
{
    return buildData();
}
