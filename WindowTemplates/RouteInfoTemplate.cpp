#include "RouteInfoTemplate.h"

#include <QDebug>

using namespace ISKRA;
using namespace Window;

namespace
{
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

RouteInfoTemplate::RouteInfoTemplate(quint8 ledAddress, QObject *parent)
    : AbstractWindowTemplate(ledAddress, parent)
{}

RouteInfoTemplate::~RouteInfoTemplate()
{}

QList<QByteArray> RouteInfoTemplate::getStationNameWindow(const QString &name, int windowNum)
{
    QList<QByteArray> result;

    result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow,
                         createTitleWindow(name, windowNum)).encode());

    return result;
}

QList<QByteArray> RouteInfoTemplate::getAdditionalWindow(int firstWindowNum, bool showTemp, int temperature, Window::Color tempColor)
{
    QList<QByteArray> result;

    QList<QByteArray> rawData = createAdditionalInfoWindow(firstWindowNum, showTemp, temperature, tempColor);

    for (const auto& data : qAsConst(rawData))
        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow, data).encode());

    return result;
}

QList<QByteArray> RouteInfoTemplate::getArrivingVehicleWindow(const VehicleInfo &info, int firstWindowNum, int numOfCreatedWindow)
{
    QList<QByteArray> result;
    QList<QByteArray>  rawData = createArrivingVehicleWindow(info, firstWindowNum, numOfCreatedWindow);

    for (const auto& data : rawData)
        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow, data).encode());

    return result;
}

void RouteInfoTemplate::reset()
{
    NUMBER_OF_ADD = 0;
    NUMBER_OF_CREATED_ROUTE_WINDOW = 0;
    m_stationInfo = StationInfo();
}



QList<QByteArray> RouteInfoTemplate::createAdditionalInfoWindow(int firstWindowNum, bool showTemp,
                                                                int temperature, Window::Color tempColor)
{
    QList<QByteArray> result;

    int currentWindowNum = firstWindowNum;

    WindowCreator timeWindow;
    timeWindow.setNumber(currentWindowNum);
    timeWindow.setColor(WHITE, TRANSPARENT);
    timeWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    timeWindow.setAbsoluteHorizontalPosition(0);
    timeWindow.setAbsoluteWidth(TIME_WINDOW_WITH);
    timeWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    timeWindow.setFont(FONT_NUMBER);
    timeWindow.setStartState(StartState::Appears);
    timeWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    timeWindow.setText(InlineCommands::showHoursWord() + ":" + InlineCommands::showMinutesWord("01"));
    timeWindow.setAutoScrolling(0);
    timeWindow.setNumberOfRepeat(0);
    timeWindow.setDuration(0);
    timeWindow.setShowAndClearSpeed(8, 8);

    result.append(timeWindow.getData());

    currentWindowNum += 1;

    WindowCreator dateWindow;
    dateWindow.setNumber(currentWindowNum);

    dateWindow.setColor(tempColor, TRANSPARENT);
    dateWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    dateWindow.setAbsoluteHorizontalPosition(TIME_WINDOW_WITH);
    dateWindow.setAbsoluteWidth(LED_WIDTH - TIME_WINDOW_WITH - ARIVE_MIN_TITLE_WITH);
    dateWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    dateWindow.setFont(FONT_NUMBER);
    dateWindow.setStartState(StartState::Appears);
    dateWindow.setShowAndClearEffect(Effect::StaticText, Effect::StaticText);
    dateWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);

    if (showTemp) dateWindow.setText(QString::number(temperature) + "°С");
    else
        dateWindow.setText(InlineCommands::showDayWord("01") + "." + InlineCommands::showMounthWord("01")
                           + ".21");

    dateWindow.setAutoScrolling(0);
    dateWindow.setNumberOfRepeat(0);
    dateWindow.setDuration(0);
    dateWindow.setShowAndClearSpeed(8, 8);

    result.append(dateWindow.getData());

    currentWindowNum += 1;

    WindowCreator minTitleWindow;
    minTitleWindow.setNumber(currentWindowNum);

    minTitleWindow.setColor(WHITE, TRANSPARENT);
    minTitleWindow.setAbsoluteHorizontalPosition(LED_WIDTH - ARIVE_MIN_TITLE_WITH);
    minTitleWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING);
    minTitleWindow.setAbsoluteWidth(ARIVE_MIN_TITLE_WITH);
    minTitleWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    minTitleWindow.setFont(FONT_NUMBER);
    minTitleWindow.setStartState(StartState::Appears);
    minTitleWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Right);
    minTitleWindow.setText("Мин.");
    minTitleWindow.setAutoScrolling(0);
    minTitleWindow.setNumberOfRepeat(0);
    minTitleWindow.setDuration(0);
    minTitleWindow.setShowAndClearSpeed(8, 8);

    result.append(minTitleWindow.getData());

    return result;
}

QList<QByteArray> RouteInfoTemplate::createArrivingVehicleWindow(const VehicleInfo &info, int firstWindowNum, int numOfCreatedWindow)
{
    QList<QByteArray> result;

    int currentWindowNum = firstWindowNum;

    WindowCreator numberWindow;

    numberWindow.setNumber(currentWindowNum);
    numberWindow.setColor(WHITE, TRANSPARENT);
    numberWindow.setAbsoluteHorizontalPosition(2);
    numberWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING * (numOfCreatedWindow + 2));
    numberWindow.setAbsoluteWidth(ROUTE_NUM_WINDOW_WITH);
    numberWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    numberWindow.setFont(FONT_NUMBER);
    numberWindow.setStartState(StartState::Appears);
    numberWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Left);
    numberWindow.setText(info.routeNum);
    numberWindow.setAutoScrolling(0);
    numberWindow.setNumberOfRepeat(0);
    numberWindow.setDuration(0);
    numberWindow.setShowAndClearSpeed(8, 8);

    result.append(numberWindow.getData());

    currentWindowNum += 1;

    WindowCreator infoWindow;

    int assignLeftWindow = firstWindowNum;
    int assignRightWindow = currentWindowNum + 1;

    infoWindow.setNumber(currentWindowNum);
    infoWindow.setColor(WHITE, TRANSPARENT);
    infoWindow.setRelativeHorizontalPosition(PositionForX::Right, assignLeftWindow, Relation::Window);
    infoWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING * (numOfCreatedWindow + 2));
    infoWindow.setRelativeWidth(RelativeWidth::ToLeftCorner, assignRightWindow, Relation::Window);
    infoWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    infoWindow.setFont(FONT_NUMBER);
    infoWindow.setStartState(StartState::Appears);
    infoWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Left);
    infoWindow.setText(info.endStation);
    infoWindow.setAutoScrolling(2);
    infoWindow.setNumberOfRepeat(0);
    infoWindow.setDuration(0);
    infoWindow.setShowAndClearSpeed(8, 8);

    result.append(infoWindow.getData());

    currentWindowNum += 1;

    WindowCreator timeToArriveWindow;

    timeToArriveWindow.setNumber(currentWindowNum);
    timeToArriveWindow.setColor(info.timeToArrive == 0 ? GREEN : WHITE, TRANSPARENT);
    timeToArriveWindow.setAbsoluteHorizontalPosition(LED_WIDTH - MIN_TO_ARRIVE_WINDOW_WITH);
    timeToArriveWindow.setVerticalPosition(SECOND_LINE_TOP_PADDING * (numOfCreatedWindow + 2));
    timeToArriveWindow.setAbsoluteWidth(MIN_TO_ARRIVE_WINDOW_WITH);
    timeToArriveWindow.setHeight(HORIZONTAL_SEGMENT_HEIGHT);
    timeToArriveWindow.setFont(FONT_NUMBER);
    timeToArriveWindow.setStartState(StartState::Appears);
    timeToArriveWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    timeToArriveWindow.setText(QString::number(info.timeToArrive));
    timeToArriveWindow.setAutoScrolling(0);
    timeToArriveWindow.setNumberOfRepeat(0);
    timeToArriveWindow.setDuration(0);
    timeToArriveWindow.setShowAndClearSpeed(8, 8);

    result.append(timeToArriveWindow.getData());

    return result;
}

QByteArray RouteInfoTemplate::createTitleWindow(const QString &titleText, int windowNum)
{
    WindowCreator creator;
    creator.setNumber(windowNum);

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






