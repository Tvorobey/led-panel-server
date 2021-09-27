//Local
#include "AbstractWindowTemplate.h"

AbstractWindowTemplate::AbstractWindowTemplate(quint8 ledAddress, QObject *parent)
    : QObject(parent),
      m_ledAddress(ledAddress)
{}

QList<QByteArray> AbstractWindowTemplate::getData()
{
    return buildData();
}
