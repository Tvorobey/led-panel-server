#pragma once

//Qt
#include <QByteArray>
#include <QList>
#include <QPair>

/*!
 * \brief The BrightnessAdjustmentTableCreator - класс который собирает
 * таблицу представлений яркости от освещенности
 */

/*
 * NOTE: В нашем табло нет датчиков освещенности, так что нет никакого смысла
 * загружать в него данную таблицу. Если приедет табло с датчиками, то
 * в класс IskraSimpleCommand легко можно добавить данный класс
 */

class BrightnessAdjustmentTableCreator
{
public:
    explicit BrightnessAdjustmentTableCreator();
    void setBrightnessAdjustmentTable(const QList<QPair<int, int>> table);
    QByteArray getData() const;
private:
    QByteArray buildData() const;
private:
    QList<QPair<int, int>>  m_table;
};


