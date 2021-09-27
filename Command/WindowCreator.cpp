#include "WindowCreator.h"

#include <QDataStream>
#include <QTextCodec>
#include <QDebug>

using namespace Window;

namespace
{
    const Color DEFAULT_TEXT_COLOR = Color(255, 252, 185, 0);
    const Color DEFAULT_BGN_COLOR = Color(255, 0, 0, 0);

    auto standartHandler = [](QString::const_iterator& it, const uint16_t cmd)
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        QString str = *it;

        str += *(++it);

        uint16_t value = str.toInt();

        stream << uint16_t(cmd);

        stream << value;

        return data;
    };

    auto standartRouteInfoHandler = [](QString::const_iterator& it, const uint16_t cmd,
                                       uint16_t& textLenght)
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream << uint16_t(0x001D) << uint16_t(cmd);

        QString str = *it;

        str += *(++it);

        uint16_t value = str.toInt();

        stream << value;

        textLenght += 1;

        stream << uint16_t(0x001F);

        textLenght += 1;

        return data;
    };
}

WindowCreator::WindowCreator()
    :   m_windowPositionX(0),
        m_windowPositionY(0),
        m_defaultNumOfFont(0),
        m_showEffect(Effect::StaticText),
        m_clearEffect(Effect::StaticText),
        m_startState(2),
        m_numberOfRepeat(0),
        m_duration(0),
        m_showSpeed(8),
        m_clearSpeed(8),
        m_autoScrolling(1),
        m_reserve1(0),
        m_reserve2(0)
{
    setColor(DEFAULT_TEXT_COLOR, DEFAULT_BGN_COLOR);

    setTextAlignment(TextVAlignment::Center, TextHAlignment::Center);

    std::fill_n(m_linkedWindows, 32, 0xFFFF);

    m_textHandler.insert(CHANGE_FONT_WORD, [](QString::const_iterator& it)
    {
        return standartHandler(it, 0x0001);
    });

    // +
    m_textHandler.insert(CHANGE_TEXT_COLOR_WORD, [this](QString::const_iterator& it)
    {
        m_textLenght -= 8; // Количество символов в строке, соответствующие AARRGGBB

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        bool ok;

        QString alphaStr = *it;
        alphaStr += *(++it);
        uint16_t alpha  = alphaStr.toInt(&ok, 16);

        QString redStr = *(++it);
        redStr += *(++it);
        uint16_t red  = redStr.toInt(&ok, 16);

        QString greenStr = *(++it);
        greenStr += *(++it);
        uint16_t green  = greenStr.toInt(&ok, 16);

        QString blueStr = *(++it);
        blueStr += *(++it);
        uint16_t blue  = blueStr.toInt(&ok, 16);

        Color color(alpha, red, green, blue);

        stream << uint16_t(0x0002);
        m_textLenght +=1;
        stream << color.get32BitColor();
        m_textLenght += 2;  // Каждыц цвет идет одним байтом, по этому считаем
                            // что это не полноценный размер, а половина
                            // ведь один символ считается, как два байта

        return data;
    });

    //+
    m_textHandler.insert(CHANGE_BGN_COLOR_WORD, [this](QString::const_iterator& it)
    {
        m_textLenght -= 8; // Количество символов в строке, соответствующие AARRGGBB

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        bool ok;

        QString alphaStr = *it;
        alphaStr += *(++it);
        uint16_t alpha  = alphaStr.toInt(&ok, 16);

        QString redStr = *(++it);
        redStr += *(++it);
        uint16_t red  = redStr.toInt(&ok, 16);

        QString greenStr = *(++it);
        greenStr += *(++it);
        uint16_t green  = greenStr.toInt(&ok, 16);

        QString blueStr = *(++it);
        blueStr += *(++it);
        uint16_t blue  = blueStr.toInt(&ok, 16);

        Color color(alpha, red, green, blue);

        stream << uint16_t(0x0003);
        m_textLenght +=1;

        stream << color.get32BitColor();
        m_textLenght += 2;  // Каждыц цвет идет одним байтом, по этому считаем
                            // что это не полноценный размер, а половина
                            // ведь один символ считается, как два байта

        return data;
    });

    m_textHandler.insert(CHANGE_INTERSYMBOL_MISMATCH_WORD, [](QString::const_iterator& it)
    {
        return standartHandler(it, 0x0004);
    });

    m_textHandler.insert(CHANGE_VERTICAL_BIAS_WORD, [](QString::const_iterator& it)
    {
        return standartHandler(it, 0x0005);
    });

    m_textHandler.insert(FLEXIBLE_SPACE_WORD, [](QString::const_iterator& it)
    {
        return standartHandler(it, 0x0006);
    });

    m_textHandler.insert(SET_IMAGE_WORD, [](QString::const_iterator& it)
    {
        return standartHandler(it, 0x0007);
    });

    m_textHandler.insert(SHOW_ROUTE_NUMBER_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0001, m_textLenght);
    });

    m_textHandler.insert(SHOW_START_STATION_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0002, m_textLenght);
    });

    m_textHandler.insert(SHOW_END_STATION_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0003, m_textLenght);
    });

    m_textHandler.insert(SHOW_STATION_NAME_WORD, [this](QString::const_iterator& it)
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        m_textLenght -=4;   // Вычитаем количество символов, которые соответсвуют номеру языка
                            // И номеру остановки

        stream << uint16_t(0x001D) << uint16_t(0x0004);

        m_textLenght +=2;

        QString languageNumStr = *it;
        languageNumStr += *(++it);
        uint16_t languageNum = languageNumStr.toInt();

        QString stationNumStr = *(++it);
        stationNumStr += *(++it);
        uint16_t stationNum = stationNumStr.toInt();

        stream << languageNum << stationNum;

        m_textLenght += 2;

        stream << uint16_t(0x001F);

        m_textLenght += 1;

        return data;
    });

    //+
    // TODO: переписать функцию нормально и понятно. Потому что тут проиходит магия,конечно, но логичная
    // NOTE: сепаратор может быть либо "\n" либо "-"
    m_textHandler.insert(SHOW_STATION_LIST_WORD, [this](QString::const_iterator& it)
    {
        m_textLenght -= 6;  // Удаляем из размера 7 символов, а именно:
                            // хх - номер языка (size = 2)
                            // уу, zz - количество станций и тип (size = 4)

        // WARNING: Решение косяковое, надо подумать над тем, как нормально записывать
        // и в 16-LE и данные в байт эррей Спросить у Григория Борисовича или ребят
        QByteArray data;

        QByteArray prevData;
        QDataStream stream(&prevData, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream << uint16_t(0x001D) << uint16_t(0x0005);
        m_textLenght +=2;

        QString languageNumStr = *it;
        languageNumStr += *(++it);
        uint16_t languageNum = languageNumStr.toInt();

        stream << languageNum;
        m_textLenght += 1;

        QTextCodec* codec = QTextCodec::codecForName("UTF-16LE");

        QString separator;

        while (*(++it) != "|")
        {
            separator.append(*it);
        }

        QByteArray stringData = codec->fromUnicode(separator).remove(0, 2);

        bool ok;
        QString showTypeStr = *(++it);
        showTypeStr += *(++it);
        uint8_t showType = showTypeStr.toInt(&ok, 16);

        QString showLimitStr = *(++it);
        showLimitStr += *(++it);
        uint8_t showLimit = showLimitStr.toInt(&ok, 16);

        uint16_t stationParams = 0;
        stationParams |= showLimit;
        stationParams |= showType << 8;

        QByteArray endData;
        QDataStream secondStream(&endData, QIODevice::WriteOnly | QIODevice::Append);
        secondStream.setByteOrder(QDataStream::LittleEndian);

        uint16_t verticalLine = 0x0000;

        secondStream << verticalLine << stationParams << uint16_t(0x001F);
        m_textLenght += 2;

        data.append(prevData).append(stringData).append(endData);

        return data;
    });

    m_textHandler.insert(SHOW_CURRENT_STATION_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0007, m_textLenght);
    });

    m_textHandler.insert(SHOW_NEXT_STATION_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0008, m_textLenght);
    });

    m_textHandler.insert(SHOW_DAY_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0013, m_textLenght);
    });

    m_textHandler.insert(SHOW_MOUNTH_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0014, m_textLenght);
    });

    m_textHandler.insert(SHOW_YEAR_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0015, m_textLenght);
    });

    m_textHandler.insert(SHOW_HOURS_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0010, m_textLenght);
    });

    m_textHandler.insert(SHOW_MINUTES_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0011, m_textLenght);
    });

    m_textHandler.insert(SHOW_SECONDS_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0012, m_textLenght);
    });

    m_textHandler.insert(SHOW_DAY_OF_THE_WEEK_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0016, m_textLenght);
    });


    m_textHandler.insert(SHOW_MOUNTH_TEXT_NAME_WORD, [this](QString::const_iterator& it)
    {
        return standartRouteInfoHandler(it, 0x0017, m_textLenght);
    });
}

QByteArray WindowCreator::getData() const
{
    return buildData();
}

void WindowCreator::setAbsoluteHorizontalPosition(const int posX)
{
    m_windowPositionX = posX;
}

void WindowCreator::setRelativeHorizontalPosition(const PositionForX pos, const quint8 window,
                                                        const Relation relation)
{
    m_windowPositionX = 0;

    m_windowPositionX = 1 << 15 | quint8(relation) << 7 | window << 2 | quint8(pos);
}

void WindowCreator::setVerticalPosition(const int posY)
{
    m_windowPositionY = 0;
    m_windowPositionY = posY;
}

void WindowCreator::setNumber(const int number)
{
    m_windowNumber = number;
}

void WindowCreator::setAbsoluteWidth(const int width)
{
    m_windowWidth = 0;
    m_windowWidth = width;
}

void WindowCreator::setRelativeWidth(const RelativeWidth width, const quint8 window, const Relation relation)
{
    m_windowWidth = 0;

    m_windowWidth = 1 << 15 | quint8(relation) << 7 | window << 2 | quint8(width);
}

void WindowCreator::setHeight(const int height)
{
    m_windowHeight  = height;
}

void WindowCreator::setColor(const Window::Color& textColor, const Window::Color& bgnColor)
{
    m_defaultTextColor  = textColor.get32BitColor();
    m_defaultBgnColor   = bgnColor.get32BitColor();
}

void WindowCreator::setFont(const int fontNum)
{
    m_defaultNumOfFont = fontNum;
}

void WindowCreator::setShowAndClearEffect(const Window::Effect show, const Window::Effect clear)
{
    m_showEffect    = show;
    m_clearEffect   = clear;
}

void WindowCreator::setStartState(const Window::StartState state)
{
    m_startState = static_cast<uint8_t>(state);
}

void WindowCreator::setText(const QString &text)
{
    int specSimbolCount = text.count("\\");
    m_text = text;
    m_textLenght = text.size() - specSimbolCount*2;   // Умножаем на два, так как после спец символа
                                                    // еще идет индетификатор, который не
                                                    // учавствует в строке
 }

void WindowCreator::setNumberOfRepeat(const int count)
{
    m_numberOfRepeat = count;
}

void WindowCreator::setDuration(const int duration)
{
    m_duration = duration;
}

void WindowCreator::setShowAndClearSpeed(const int showSpeed, const int clearSpeed)
{
    m_showSpeed     = showSpeed;
    m_clearSpeed    = clearSpeed;
}

void WindowCreator::setAutoScrolling(const int value)
{
    m_autoScrolling = value;
}

void WindowCreator::setTextAlignment(const Window::TextVAlignment VAlign, const Window::TextHAlignment HAlign)
{
    m_textAlignment = 0;
    m_textAlignment = quint8(VAlign) << 4 | quint8(HAlign);
}

void WindowCreator::setLinkedWindow(const int num, Window::StartState onShowState, Window::StartState beforeClearState, Window::StartState afterClearState)
{
    m_linkedWindows[num] = 0x0000;
    m_linkedWindows[num] |= static_cast<uint8_t>(afterClearState);
    m_linkedWindows[num] |= static_cast<uint8_t>(beforeClearState) << 4;
    m_linkedWindows[num] |= static_cast<uint8_t>(onShowState) << 8;
    m_linkedWindows[num] |= 0xF << 12;
}

QByteArray WindowCreator::buildData() const
{
    QByteArray textData;

    QTextCodec* codec = QTextCodec::codecForName("UTF-16LE");

    for (QString::const_iterator it = m_text.begin(); it != m_text.end(); ++it)
    {
        if (*it != "\\")
            textData.append(codec->fromUnicode(*it).remove(0, 2));
        else
        {
            QString keyWord = *it;
            keyWord += *(++it);
            ++it;
            textData.append(m_textHandler.value(keyWord)(it));
        }
    }

    QByteArray data;

    QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << m_windowNumber;
    stream << m_windowPositionX;
    stream << m_windowPositionY;
    stream << m_windowWidth;
    stream << m_windowHeight;
    stream << m_defaultTextColor;
    stream << m_defaultBgnColor;
    stream << m_defaultNumOfFont;
    stream << static_cast<uint8_t>(m_showEffect);
    stream << static_cast<uint8_t>(m_clearEffect);
    stream << m_startState;
    stream << m_textLenght;
    stream << m_numberOfRepeat;
    stream << m_duration;
    stream << m_showSpeed;
    stream << m_clearSpeed;
    stream << m_autoScrolling;
    stream << m_reserve1;
    stream << m_reserve2;
    stream << m_textAlignment;

    for (int i = 0; i < 32; i ++)
        stream << m_linkedWindows[i];

    data.append(textData);

    return data;
}
