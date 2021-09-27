#pragma once

//QT
#include <QVector>
#include <QMap>
//STD
#include <functional>

namespace Window
{
    class Color
    {
    public:
        explicit Color(uint8_t alfa, uint8_t red, uint8_t green, uint8_t blue)
            : m_alfa(alfa), m_red(red), m_green(green), m_blue(blue)
        {}

        uint32_t get32BitColor() const
        {
            uint32_t color32Bit = 0;
            color32Bit |= m_blue;
            color32Bit |= m_green << 8;
            color32Bit |= m_red << 16;
            color32Bit |= m_alfa << 24;

            return color32Bit;
        }
    private:
        uint8_t m_alfa;
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
    };

    enum class Effect : uint8_t
    {
        StaticText = 0,
        Ticker,
        Scrolling,
        MovementToTheLeft,
        MovementToTheRight,
        MovementToTheUp,
        MovementToTheDown,
        LeftCurtain,
        RightCurtain,
        DownCurtain,
        UpCurtain,
        Blocks,
        Opacity,
        Zoom,
        FromDot
    };

    enum class TextVAlignment : uint8_t
    {
        Top = 0,
        Center,
        Bottom
    };

    enum class TextHAlignment : uint8_t
    {
        Left = 0,
        Center,
        Right
    };

    // Данное перечисление справедливо и для нового состояния окна, при работе
    // со связанными окнами
    enum class StartState : uint8_t
    {
        NoExist = 0,
        NoActive,
        Appears,
        IsDisplayed,
        Hides,
        DoNotChange = 0xF
    };

    enum class Relation : quint8
    {
        Screen = 0,
        Window
    };

    enum class PositionForX : uint8_t
    {
        Left = 0,
        Right,
        RightCornerOfWindow, // Правый край окна будет совпадать с правым краем указанного окна
        DoNotUse
    };

    enum class RelativeWidth : size_t
    {
        ToLeftCorner = 0,
        ToRightCorner,
        FlexByText,
        TheSameThatWindow // Длинна окна такая же, как у выбранного окна
    };

    const QString CHANGE_FONT_WORD                  = "\\f";
    const QString CHANGE_TEXT_COLOR_WORD            = "\\t";
    const QString CHANGE_BGN_COLOR_WORD             = "\\b";
    const QString CHANGE_INTERSYMBOL_MISMATCH_WORD  = "\\s";
    const QString CHANGE_VERTICAL_BIAS_WORD         = "\\o";
    const QString FLEXIBLE_SPACE_WORD               = "\\_";
    const QString SET_IMAGE_WORD                    = "\\i";
    const QString SHOW_ROUTE_NUMBER_WORD            = "\\#";
    const QString SHOW_START_STATION_WORD           = "\\F";
    const QString SHOW_END_STATION_WORD             = "\\L";
    const QString SHOW_STATION_NAME_WORD            = "\\B";
    const QString SHOW_STATION_LIST_WORD            = "\\l";
    const QString SHOW_CURRENT_STATION_WORD         = "\\C";
    const QString SHOW_NEXT_STATION_WORD            = "\\N";
    const QString SHOW_DAY_WORD                     = "\\D";
    const QString SHOW_MOUNTH_WORD                  = "\\m";
    const QString SHOW_YEAR_WORD                    = "\\Y";
    const QString SHOW_HOURS_WORD                   = "\\H";
    const QString SHOW_MINUTES_WORD                 = "\\M";
    const QString SHOW_SECONDS_WORD                 = "\\S";
    const QString SHOW_DAY_OF_THE_WEEK_WORD         = "\\W";
    const QString SHOW_MOUNTH_TEXT_NAME_WORD        = "\\T";

    class InlineCommands
    {
    public:
        explicit InlineCommands();

        // TODO: добавить проверку на правильность ввода строки. Должно быть условно XX. assert
        static QString changeFontWord(const QString& fontNum = "00")
        {
            return QString(CHANGE_FONT_WORD + fontNum);
        }
        static QString changeTextColorWord(const QString& alphaHex,
                                           const QString& redHex,
                                           const QString& greenHex,
                                           const QString& blueHex)
        {
            return QString(CHANGE_TEXT_COLOR_WORD + alphaHex + redHex + greenHex + blueHex);
        }
        static QString changeBgnColorWord(const QString& alphaHex,
                                          const QString& redHex,
                                          const QString& greenHex,
                                          const QString& blueHex)
        {
            return QString(CHANGE_BGN_COLOR_WORD + alphaHex + redHex + greenHex + blueHex);
        }
        static QString changeIntersymbolMismatchWord(const QString& mismatch = "00")
        {
            return QString(CHANGE_INTERSYMBOL_MISMATCH_WORD + mismatch);
        }
        static QString changeVerticalBiasWord(const QString& bias = "00")
        {
            return QString(CHANGE_VERTICAL_BIAS_WORD + bias);
        }
        static QString flexibleSpaceWord(const QString& width = "00")
        {
            return QString(FLEXIBLE_SPACE_WORD + width);
        }
        static QString imageWord(const QString& num = "00")
        {
            return QString(SET_IMAGE_WORD + num);
        }
        static QString showRouteNumberWord(const QString& languageNum = "00")
        {
            return QString(SHOW_ROUTE_NUMBER_WORD + languageNum);
        }
        static QString showStartStationWord(const QString& languageNum = "00")
        {
            return QString(SHOW_START_STATION_WORD + languageNum);
        }
        static QString showEndStationWord(const QString& languageNum = "00")
        {
            return QString(SHOW_END_STATION_WORD + languageNum);
        }
        static QString showStationNameWord(const QString& languageNum = "00",
                                           const QString& stationNum = "01")
        {
            return QString(SHOW_STATION_NAME_WORD + languageNum + stationNum);
        }
        static QString showStationListWord(const QString& languageNum = "00",
                                           const QString& separator = "-",
                                           const QString& isAllHex = "00",
                                           const QString& visibleStationCountHex = "0A")
        {
            return QString(SHOW_STATION_LIST_WORD + languageNum + separator + "|"
                           + isAllHex + visibleStationCountHex);
        }
        static QString showCurrentStationWord(const QString& languageNum = "00")
        {
            return QString(SHOW_CURRENT_STATION_WORD + languageNum);
        }
        static QString showNextStationWord(const QString& languageNum = "00")
        {
            return QString(SHOW_NEXT_STATION_WORD + languageNum);
        }
        static QString showDayWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_DAY_WORD + showLeadingZeroes);
        }
        static QString showMounthWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_MOUNTH_WORD + showLeadingZeroes);
        }
        static QString showYearWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_YEAR_WORD + showLeadingZeroes);
        }
        static QString showHoursWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_HOURS_WORD + showLeadingZeroes);
        }
        static QString showMinutesWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_MINUTES_WORD + showLeadingZeroes);
        }
        static QString showSecondsWord(const QString& showLeadingZeroes = "00")
        {
            return QString(SHOW_SECONDS_WORD + showLeadingZeroes);
        }
        static QString showDayOfTheWeekWord(const QString& numOfTable = "00")
        {
            return QString(SHOW_DAY_OF_THE_WEEK_WORD + numOfTable);
        }
        static QString showMounthTextNameWord(const QString& numOfTable = "00")
        {
            return QString(SHOW_MOUNTH_TEXT_NAME_WORD + numOfTable);
        }
    };
}

typedef  QMap<QString, std::function<QByteArray(QString::const_iterator& it)>> TextHandlerMap;

class WindowCreator
{
public:
    explicit WindowCreator();
    QByteArray getData() const;
    void setAbsoluteHorizontalPosition(const int posX);
    void setRelativeHorizontalPosition(const Window::PositionForX pos, const quint8 window,
                                             const Window::Relation relation);
    void setVerticalPosition(const int posY);
    void setNumber(const int number);
    void setAbsoluteWidth(const int width);
    /*!
     * \brief setRelativeWidth - устанавливает относительную длинну окна. Если указан параметр
     * Relation::Screen, то номер окна не учитывается (можно вписать любой)
     */
    void setRelativeWidth(const Window::RelativeWidth width, const quint8 window,
                                const Window::Relation relation);
    void setHeight(const int height);
    void setColor(const Window::Color& textColor, const Window::Color& bgnColor);
    void setFont(const int fontNum);
    void setShowAndClearEffect(const Window::Effect show, const Window::Effect clear);
    void setStartState(const Window::StartState state);
    void setText(const QString& text);
    void setNumberOfRepeat(const int count);
    /*!
     * \brief setDuration - устанавливает время отображение окна
     * \param duration - если 0 - то отображается бесконечно
     * Работает только если эффект скрытия не StaticText
     */
    void setDuration(const int duration);
    void setShowAndClearSpeed(const int showSpeed, const int clearSpeed);
    /*!
     * \brief setAutoScrolling - автоматическая бегущая строка
     * (не работает с эффектами "бегущая строка" и "скроллинг")
     * \param value 0 - не используется, 1 - скорость движения бегущей строки
     */
    void setAutoScrolling(const int value);
    void setTextAlignment(const Window::TextVAlignment VAlign, const Window::TextHAlignment HAlign);
    void setLinkedWindow(const int num, Window::StartState onShowState,
                         Window::StartState beforeClearState,
                         Window::StartState afterClearState);
private:
    QByteArray buildData() const;
private:
    uint8_t         m_windowNumber;
    uint16_t        m_windowPositionX;
    uint16_t        m_windowPositionY;
    uint16_t        m_windowWidth;
    uint16_t        m_windowHeight;
    uint32_t        m_defaultTextColor;
    uint32_t        m_defaultBgnColor;
    uint8_t         m_defaultNumOfFont;
    Window::Effect  m_showEffect;
    Window::Effect  m_clearEffect;
    uint8_t         m_startState;
    uint16_t        m_textLenght;
    uint16_t        m_numberOfRepeat;
    /*!
     * \brief m_duration - ремя отображения окна от момента появления до скрытия
     *  0 - бесконечный показ
     */
    uint16_t        m_duration;
    uint8_t         m_showSpeed;
    uint8_t         m_clearSpeed;
    uint8_t         m_autoScrolling;
    uint8_t         m_reserve1;
    uint8_t         m_reserve2;
    uint8_t         m_textAlignment;
    /*!
     * \brief m_linkedWindows - Слово указывающее в какое состояние должно перетйи целевое окно, после события
     *  [0...3] - Событие "После отображения"
     *  [4...7] - Событие "Перед стиранием"
     *  [8...11] - Событие "После стирания"
     *  [12...15] - Резерв - всегда 0xF
     */
    uint16_t        m_linkedWindows[32];
    QString         m_text;

    TextHandlerMap  m_textHandler;
};


