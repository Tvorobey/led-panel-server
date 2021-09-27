#pragma once

//Qt
#include <QByteArray>
#include <QVector>

struct ImgFormat
{
    ImgFormat()
        : width(0),
          height(0),
          colorDepth(0),
          numberOfFrame(0),
          animationType(0)
    {}

    uint16_t            width;
    uint16_t            height;
    uint8_t             colorDepth;
    uint8_t             numberOfFrame;
    uint8_t             animationType;
    QVector<uint8_t>    delay;
    QByteArray          startAlignment;
    QByteArray          imgBMP;
    QByteArray          endAlignment;
};

class ImageLibraryCreator
{
public:
    explicit ImageLibraryCreator();
    QByteArray getData() const;
    void clearImageLibrary();
    void appendImage(const QString& fileName);
    void appendImage(const QByteArray& data);
private:
    QByteArray  getByteArrayFromFile(const QString& fileName);
    void        handleImageAndAppend(const QByteArray& data);
private:
    uint8_t m_imgNumber;
    QList<ImgFormat> m_images;
};


