#include "PresentationWindowTemplate.h"

#include <QDebug>

using namespace ISKRA;
using namespace Window;

namespace
{
    const Window::Color WHITE   = Window::Color(255, 255, 255, 255);
    const Window::Color BLUE    = Window::Color(10, 62, 0, 255);
    const Window::Color BLACK   = Window::Color(255, 0, 0, 0);
    const Window::Color GREEN   = Window::Color(255, 1, 254, 0);
    const Window::Color PIRPLE  = Window::Color(255, 213, 27, 229);
    const Window::Color RED     = Window::Color(255, 255, 0, 0);
    const Window::Color TRANSPARENT = Window::Color(0, 0, 0, 0);

    const QString BTN_INFO_WINDOW                   = "BtnInfoWindow";
    const QString BGN_WINDOW                        = "BgnWindow";
    const QString TITLE_WINDOW                      = "TitleWindow";
    const QString TIME_WINDOW                       = "TimeWindow";
    const QString TEMPERATURE_WINDOW                = "TemperatureWindow";
    const QString TIME_TITLE_WINDOW                 = "TimeTitleWindow";
    const QString FIRST_ROUTE_NUM_WINDOW            = "FirstRouteNumWindow";
    const QString ROUTE_ICON_NUM_WINDOW             = "RouteIconWindow";
    const QString FIRS_ROUTE_INFO_WINDOW            = "FirstCreatedRouteInfoWindow";
    const QString FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW = "FirstCreatedTimToArriveWindow";
    const QString ADDITIONAL_WINDOW                 = "AdditioanlWindow";
    const QString LAST                              = "Last";

    constexpr int FONT_NUMBER = 0;
    constexpr int FONT_NUMBER_BIG = 1;

    constexpr int MAX_ROUTE_INFO_WINDOW = 6;

    int NUMBER_OF_CREATED_ROUTE_WINDOW    = 0;
    int NUMBER_OF_ADD = 0;
    int ROUTES_WINDOWS_SHIFT = 0;


    constexpr int LED_WIDTH  = 128;
    constexpr int LED_HEIGHT = 128;


    constexpr int ROUTE_NUM_WINDOW_WITH = 24;
    constexpr int HORIZONTAL_SEGMENT_HEIGHT = 16;
    constexpr int TIME_WINDOW_WITH = 25;
    constexpr int ARIVE_MIN_TITLE_WITH = 20;
    constexpr int SECOND_LINE_TOP_PADDING = 17;
    constexpr int MIN_TO_ARRIVE_WINDOW_WITH = 14;
}

PresentationWindowTemplate::PresentationWindowTemplate(quint8 ledAddress, QObject *parent)
    : AbstractWindowTemplate(ledAddress, parent)
{
    /*NOTE: окно с более старшим номером, перекрывает окно с более младшим*/
    /*TODO: переделать на энум, а то порнография какая то*/
    m_windowNumsMap =
    {
        {BGN_WINDOW,                        0},
        {BTN_INFO_WINDOW,                   1},
        {TITLE_WINDOW,                      2},
        {TIME_WINDOW,                       3},
        {TEMPERATURE_WINDOW,                4},
        {ADDITIONAL_WINDOW,                 5},
        {TIME_TITLE_WINDOW,                 6},
        {FIRST_ROUTE_NUM_WINDOW,            7},
//        {ROUTE_ICON_NUM_WINDOW,             8},
        {FIRS_ROUTE_INFO_WINDOW,            8},
        {FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW, 9},
        {LAST,                              10}
    };

    ROUTES_WINDOWS_SHIFT = m_windowNumsMap.value(LAST) - m_windowNumsMap.value(FIRST_ROUTE_NUM_WINDOW);
}

PresentationWindowTemplate::~PresentationWindowTemplate()
{}

int PresentationWindowTemplate::addRouteInfo(const ArriveRouteInfo &info)
{
    ++NUMBER_OF_ADD;

    m_routesInfo.append(info);

    return m_windowNumsMap.value(FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_ADD;
}

void PresentationWindowTemplate::reset()
{
    NUMBER_OF_ADD = 0;
    NUMBER_OF_CREATED_ROUTE_WINDOW = 0;
    m_routesInfo.clear();
}

QByteArray PresentationWindowTemplate::updateArriveTime(int min, int windowNum)
{
    QByteArray result;

    result = Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         updateArriveTimeWindow(min, windowNum)).encode();

    return result;
}


QList<QByteArray> PresentationWindowTemplate::buildData()
{
    QList<QByteArray> result;

    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createBgnWindow()).encode());
    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createTitleWindow("Шлагбаум")).encode());
    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createTimeWindow()).encode());
    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createTemperatureWindow(15)).encode());
    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createMinTitleWindow()).encode());

    /*Создаем окна под маршруты*/
    for (const auto& info : qAsConst(m_routesInfo))
    {
        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                             createRouteNumWindow(info.routeNum, info.numColor)).encode());

//        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
//                                 createRouteIcon(info.iconNumber)).encode());

        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                             createRouteInfoWindow(info.routeInfo, WHITE)).encode());
        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                             createArriveTimeWindow(info.timeToArrive)).encode());
    }

    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createBtnINFO()).encode());

    return result;
}

QByteArray PresentationWindowTemplate::createBgnWindow()
{
    WindowCreator creator;
    creator.setNumber(m_windowNumsMap.value(BGN_WINDOW));

    creator.setColor(WHITE, BLACK);
    creator.setVerticalPosition(0);
    creator.setAbsoluteHorizontalPosition(0);
    creator.setAbsoluteWidth(LED_WIDTH);
    creator.setHeight(LED_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText("");
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createTitleWindow(const QString &titleText)
{
    WindowCreator creator;
    creator.setNumber(m_windowNumsMap.value(TITLE_WINDOW));

    creator.setColor(WHITE, TRANSPARENT);
    creator.setVerticalPosition(0);
    creator.setAbsoluteHorizontalPosition(0);
    creator.setAbsoluteWidth(LED_WIDTH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER_BIG);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(titleText);
    creator.setAutoScrolling(5);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createTimeWindow()
{
    WindowCreator creator;
    creator.setNumber(m_windowNumsMap.value(TIME_WINDOW));

    creator.setColor(WHITE, TRANSPARENT);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    creator.setAbsoluteHorizontalPosition(0);
    creator.setAbsoluteWidth(TIME_WINDOW_WITH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(InlineCommands::showHoursWord() + ":" + InlineCommands::showMinutesWord("01"));
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

/*Теперь выводит дату*/
QByteArray PresentationWindowTemplate::createTemperatureWindow(int temp)
{
    Q_UNUSED(temp)

    WindowCreator creator;
    creator.setNumber(m_windowNumsMap.value(TEMPERATURE_WINDOW));

    creator.setColor(WHITE, TRANSPARENT);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    creator.setAbsoluteHorizontalPosition(TIME_WINDOW_WITH);
    creator.setAbsoluteWidth(LED_WIDTH - TIME_WINDOW_WITH - ARIVE_MIN_TITLE_WITH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(InlineCommands::showDayWord("01") + "." + InlineCommands::showMounthWord("01")
                     + ".21");
//    creator.setText("+" + QString::number(temp) + "°");
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createMinTitleWindow()
{
    WindowCreator creator;
    creator.setNumber(m_windowNumsMap.value(TIME_TITLE_WINDOW));

    creator.setColor(WHITE, TRANSPARENT);
    creator.setAbsoluteHorizontalPosition(LED_WIDTH - ARIVE_MIN_TITLE_WITH);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    creator.setAbsoluteWidth(ARIVE_MIN_TITLE_WITH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Right);
    creator.setText("Мин.");
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createRouteNumWindow(const QString& routeNum,
                                                            const Window::Color textColor)
{
    ++NUMBER_OF_CREATED_ROUTE_WINDOW;

    WindowCreator creator;

    int windowNum = m_windowNumsMap.value(FIRST_ROUTE_NUM_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;

    creator.setNumber(windowNum);

    creator.setColor(textColor, TRANSPARENT);
    creator.setAbsoluteHorizontalPosition(2);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING * (NUMBER_OF_CREATED_ROUTE_WINDOW + 1));
    creator.setAbsoluteWidth(ROUTE_NUM_WINDOW_WITH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Left);
    creator.setText(routeNum);
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createRouteIcon(const QString& imageNumber)
{
    WindowCreator creator;

    int windowNum = m_windowNumsMap.value(ROUTE_ICON_NUM_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;

    creator.setNumber(windowNum);

    creator.setColor(TRANSPARENT, TRANSPARENT);
    creator.setAbsoluteHorizontalPosition(ROUTE_NUM_WINDOW_WITH);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING * (NUMBER_OF_CREATED_ROUTE_WINDOW + 1));
    creator.setAbsoluteWidth(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(InlineCommands::imageWord(imageNumber));
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createBtnINFO()
{
    WindowCreator creator;

    int windowNum = m_windowNumsMap.value(BTN_INFO_WINDOW);

    creator.setNumber(windowNum);

    creator.setColor(WHITE, BLACK);
    creator.setAbsoluteHorizontalPosition(0);
    creator.setVerticalPosition(106);
    creator.setAbsoluteWidth(LED_WIDTH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER_BIG);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(QString()/*"www.orgp.spb.ru"*/);
    creator.setAutoScrolling(2);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createRouteInfoWindow(const QString &routeInfo, Window::Color textColor, int num)
{
    WindowCreator creator;

    int numberOfCreatedWindow = num == -1 ? NUMBER_OF_CREATED_ROUTE_WINDOW : num;

    int windowNum = m_windowNumsMap.value(FIRS_ROUTE_INFO_WINDOW) + ROUTES_WINDOWS_SHIFT * numberOfCreatedWindow;

//    int assignLeftWindow = m_windowNumsMap.value(ROUTE_ICON_NUM_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;
    int assignLeftWindow = m_windowNumsMap.value(FIRST_ROUTE_NUM_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;

    int assignRightWindow = m_windowNumsMap.value(FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW)
                          + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;

    creator.setNumber(windowNum);

    creator.setColor(textColor, TRANSPARENT);
    creator.setRelativeHorizontalPosition(PositionForX::Right,
                                          assignLeftWindow,
                                          Relation::Window);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING * (NUMBER_OF_CREATED_ROUTE_WINDOW + 1));
    creator.setRelativeWidth(RelativeWidth::ToLeftCorner, assignRightWindow,
                             Relation::Window);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Left);
    creator.setText(routeInfo);
    creator.setAutoScrolling(2);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::createArriveTimeWindow(int timeToArrive)
{
    WindowCreator creator;

    int windowNum = m_windowNumsMap.value(FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW) + ROUTES_WINDOWS_SHIFT * NUMBER_OF_CREATED_ROUTE_WINDOW;

    creator.setNumber(windowNum);

    creator.setColor(WHITE, TRANSPARENT);
    creator.setAbsoluteHorizontalPosition(LED_WIDTH - MIN_TO_ARRIVE_WINDOW_WITH);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING * (NUMBER_OF_CREATED_ROUTE_WINDOW + 1));
    creator.setAbsoluteWidth(MIN_TO_ARRIVE_WINDOW_WITH);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(QString::number(timeToArrive));
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    return creator.getData();
}

QByteArray PresentationWindowTemplate::updateArriveTimeWindow(int min, int windowNum)
{
    QByteArray result;

    WindowCreator creator;    

    int stringNumber = (windowNum - m_windowNumsMap.value(FIRST_ROUTE_TIME_TO_ARRIVE_WINDOW))/ROUTES_WINDOWS_SHIFT + 1;

    creator.setNumber(windowNum);

    creator.setColor(min == 0 ? RED : WHITE, TRANSPARENT);
    creator.setAbsoluteHorizontalPosition(LED_WIDTH - MIN_TO_ARRIVE_WINDOW_WITH);
//    creator.setRelativeHorizontalPosition(PositionForX::Right,
//                                          m_windowNumsMap.value(FIRST_ROUTE_NUM_WINDOW),
//                                          Relation::Window);
    creator.setVerticalPosition(SECOND_LINE_TOP_PADDING * stringNumber);
    creator.setAbsoluteWidth(MIN_TO_ARRIVE_WINDOW_WITH);
//    creator.setRelativeWidth(RelativeWidth::ToLeftCorner, m_windowNumsMap.value(TIME_TITLE_WINDOW),
//                             Relation::Window);
    creator.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    creator.setFont(FONT_NUMBER);
    creator.setStartState(StartState::Appears);
    creator.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    creator.setText(QString::number(min));
    creator.setAutoScrolling(0);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);

    result.append(creator.getData());

    return creator.getData();
}

