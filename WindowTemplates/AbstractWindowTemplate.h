#pragma once

//Qt
#include <QObject>

//Local
#include "WindowCreator.h"
#include "iskra.h"

/*!
 * \brief The AbstractWindowTemplate  - класс предназначенный для создания групп окон,
 * которые будут выступать в качестве шаблонов, для отображения информации
 */
class AbstractWindowTemplate : public QObject
{
    Q_OBJECT
public:
    explicit AbstractWindowTemplate(quint8 ledAddress, QObject *parent = nullptr);
    QList<QByteArray> getData();
    virtual void reset() = 0;
protected:
    virtual QList<QByteArray>       buildData() = 0;
    QScopedPointer<WindowCreator>   m_creator;
    QMap<QString, int>              m_windowNumsMap;
    quint8                          m_ledAddress;
};


