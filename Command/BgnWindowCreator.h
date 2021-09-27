#pragma once

//Qt
#include <QString>
#include <QByteArray>

class BgnWindowCreator
{
public:
    explicit BgnWindowCreator();
    void setImage(const QString& fileName);
    void setImage(const QByteArray& data);
    QByteArray getData() const;
private:
    QByteArray  getByteArrayFromFile(const QString& fileName);
    QByteArray  buildData() const;
private:
    QByteArray  m_image;
};


