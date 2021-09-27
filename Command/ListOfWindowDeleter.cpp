// Local
#include "ListOfWindowDeleter.h"

//Qt
#include <QDataStream>

ListOfWindowDeleter::ListOfWindowDeleter(const QList<qint8> &windows, QObject *parent)
    : QObject(parent),
      m_windowsToDelete(windows)
{}

QByteArray ListOfWindowDeleter::getData()
{
    return buildData();
}

void ListOfWindowDeleter::setWindowList(const QList<qint8> &windows)
{
    m_windowsToDelete = windows;
}

QByteArray ListOfWindowDeleter::buildData()
{
    QByteArray data;

    QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (const qint8 windowNum : m_windowsToDelete)
        stream << windowNum;

    return data;
}
