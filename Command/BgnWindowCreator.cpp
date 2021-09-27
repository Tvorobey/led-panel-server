//Local
#include "BgnWindowCreator.h"

//Qt
#include <QFile>
#include <QDebug>
#include <QImage>
#include <QBuffer>

namespace
{
    const int BMP_IMG_HEADER_LENGHT = 54;
}

BgnWindowCreator::BgnWindowCreator()
{}

void BgnWindowCreator::setImage(const QString& fileName)
{
    m_image = getByteArrayFromFile(fileName);
}

void BgnWindowCreator::setImage(const QByteArray& data)
{
    m_image = data;
}

QByteArray BgnWindowCreator::getData() const
{
    return buildData();
}

QByteArray BgnWindowCreator::getByteArrayFromFile(const QString& fileName)
{
    QFile imgFile(fileName);

    if (!imgFile.exists())
    {
        qDebug() << QString("Файла %1 - не существует").arg(fileName);
        return QByteArray();
    }

    if (!imgFile.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Ошибка в открытии файла %1, для открытия. Код ошибки %2")
                                    .arg(fileName).arg(imgFile.errorString());
        return QByteArray();
    }

    return imgFile.readAll();
}

QByteArray BgnWindowCreator::buildData() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);

    QImage image = QImage::fromData(m_image);

    qint16 width    = image.width();
    qint16 height   = image.height();
    QByteArray pixelsMatrix;

    // В чистом виде изображение выводится отзеркаленным
    // По этому поворачиваем его относительно оси Oz и Oy
    QTransform transformZ;
    transformZ.rotate(180);
    image = image.transformed(transformZ);

    QTransform transformY;
    transformY.rotate(180, Qt::YAxis);
    image = image.transformed(transformY);

    QBuffer buffer(&pixelsMatrix);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "BMP");

    // Убираем стандартный заголовок BMP файла, для табло он не нужен
    pixelsMatrix.remove(0, BMP_IMG_HEADER_LENGHT);

    stream << width << height;
    data.append(pixelsMatrix);

    return data;
}




