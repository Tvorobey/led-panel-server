//Local
#include "ImageLibraryCreator.h"

//Qt
#include <QFile>
#include <QMap>
#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QImage>

namespace
{
    enum class ColorDepth
    {
        Bit_32 = 0,
        Bit_24,
        Bit_16,
        Bit_8 = 9
    };

    const QMap<ColorDepth, int> COLOR_DEPTH_TO_BPP
    {
        {ColorDepth::Bit_32,    4},
        {ColorDepth::Bit_24,    3},
        {ColorDepth::Bit_16,    2},
        {ColorDepth::Bit_8,     1}
    };

    enum class AnimationType
    {
        NoRepeat = 0,
        Repeat
    };

    const uint8_t MOD = 4;

    const int BMP_IMG_HEADER_LENGHT = 54;

    auto calcImageSize = [](uint16_t width, uint16_t height, int bitPerPixel)
    {
        return width * height * bitPerPixel;
    };

    auto calcStartAlingment = [](uint8_t numberOfFrame)
    {
        int numberOfElement;

        numberOfElement = -(numberOfFrame - 1)%MOD;

        if (numberOfElement < 0) numberOfElement += MOD;

        return QByteArray(char(), numberOfElement);
    };

    auto calcEndAlingment = [](int imageSize)
    {
        int numberOfElement;

        numberOfElement = -imageSize%MOD;

        if (numberOfElement < 0) numberOfElement += MOD;

        return QByteArray(char(), numberOfElement);
    };
}

ImageLibraryCreator::ImageLibraryCreator()
    : m_imgNumber(0)
{}

QByteArray ImageLibraryCreator::getData() const
{
    QByteArray data;

    QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Добавляем число изображений
    stream << m_imgNumber;

    // Добавляем список изображений
    for (const auto& image : m_images)
    {
        // TODO: изменить с использованием qtendian
        QByteArray imageData;

        QDataStream imageStream(&data, QIODevice::WriteOnly | QIODevice::Append);
        imageStream.setByteOrder(QDataStream::LittleEndian);

        imageStream << image.width;
        imageStream << image.height;
        imageStream << image.colorDepth;
        imageStream << image.numberOfFrame;
        imageStream << image.animationType;

        for (const auto& delay : image.delay)
            imageStream << delay;

        imageData.append(image.startAlignment);
        imageData.append(image.imgBMP);
        imageData.append(image.endAlignment);

        data.append(imageData);
    }

    return data;
}

void ImageLibraryCreator::clearImageLibrary()
{
    m_images.clear();
}

void ImageLibraryCreator::appendImage(const QString &fileName)
{
    handleImageAndAppend(getByteArrayFromFile(fileName));
}

void ImageLibraryCreator::appendImage(const QByteArray &data)
{
    handleImageAndAppend(data);
}

QByteArray ImageLibraryCreator::getByteArrayFromFile(const QString &fileName)
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

void ImageLibraryCreator::handleImageAndAppend(const QByteArray &data)
{
    ImgFormat imgFormat;

    imgFormat.colorDepth = static_cast<uint8_t>(ColorDepth::Bit_32);
    imgFormat.numberOfFrame = 1;
    imgFormat.animationType = static_cast<uint8_t>(AnimationType::NoRepeat);
    imgFormat.delay.push_back(0);

    QImage image = QImage::fromData(data);

    imgFormat.width = image.width();
    imgFormat.height = image.height();

    // В чистом виде изображение выводится отзеркаленным
    // По этому поворачиваем его относительно оси Oz и Oy
    QByteArray arr;
    QDataStream ds(&arr, QIODevice::ReadWrite);
    ds.writeRawData((const char*)image.bits(), image.byteCount());
    ds.device()->seek(0);

    imgFormat.imgBMP = arr;

    imgFormat.startAlignment = calcStartAlingment(imgFormat.numberOfFrame);

    int imageSize = calcImageSize(imgFormat.width, imgFormat.height,
                                  COLOR_DEPTH_TO_BPP.value(ColorDepth(imgFormat.colorDepth)));

    imgFormat.endAlignment = calcEndAlingment(imageSize);

    m_images.append(imgFormat);

    ++m_imgNumber;
}
