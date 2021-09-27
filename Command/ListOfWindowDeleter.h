#pragma once

#include <QObject>

/*!
 * \brief The ListOfWindowDeleter  - класс для удаления списка окон
 * \param windows - список окон, для удаления. Передается в формате
 * "00"..."31"
 */
class ListOfWindowDeleter : public QObject
{
    Q_OBJECT
public:
    explicit ListOfWindowDeleter(const QList<qint8>& windows = {},
                                 QObject *parent = nullptr);
    QByteArray getData();
    void setWindowList(const QList<qint8>& windows);
private:
    QByteArray buildData();
private:
    QList<qint8> m_windowsToDelete;
};


