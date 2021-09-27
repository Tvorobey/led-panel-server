#include "EmergencyWindowTemplate.h"

using namespace ISKRA;
using namespace Window;

namespace
{
    constexpr int LEFT_CORNER_TITLE_WINDOW = 0;
    constexpr int TITLE_WINDOW_HEIGHT = 40;
    constexpr int WINDOW_SIZE = 128;
}

EmergencyWindowTemplate::EmergencyWindowTemplate(quint8 ledAddress, QObject *parent)
    : AbstractWindowTemplate(ledAddress, parent)
{}

QList<QByteArray> EmergencyWindowTemplate::getEmergencyInfo(int windowNum, const EmergencyInfo& info)
{
    QList<QByteArray> result;

    QList<QByteArray> rawData = buildEmergencyTemplate(windowNum, info);

    for (const auto& data : rawData)
        result.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow, data).encode());

    return result;
}

QList<QByteArray> EmergencyWindowTemplate::buildEmergencyTemplate(int windowNum, const EmergencyInfo& info)
{
    QList<QByteArray> result;

    // TODO: скрытие первого окна повесить на скрытие второго с текстом

    WindowCreator titleWindow;
    titleWindow.setNumber(windowNum);
    titleWindow.setColor(WHITE, RED);
    titleWindow.setVerticalPosition(LEFT_CORNER_TITLE_WINDOW);
    titleWindow.setAbsoluteHorizontalPosition(LEFT_CORNER_TITLE_WINDOW);
    titleWindow.setRelativeWidth(RelativeWidth::ToRightCorner, windowNum, Relation::Screen);
    titleWindow.setHeight(TITLE_WINDOW_HEIGHT);
    titleWindow.setFont(FONT_NUMBER_BIG);
    titleWindow.setStartState(StartState::Appears);
    titleWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    titleWindow.setShowAndClearEffect(Effect::Opacity, Effect::StaticText);
    titleWindow.setText(info.title);
    titleWindow.setAutoScrolling(0);
    titleWindow.setNumberOfRepeat(1);
    titleWindow.setDuration(info.showTime);
    titleWindow.setShowAndClearSpeed(255, 255);

    result.append(titleWindow.getData());

    WindowCreator textWindow;
    textWindow.setNumber(windowNum + 1);
    textWindow.setColor(WHITE, BLACK);
    textWindow.setVerticalPosition(LEFT_CORNER_TITLE_WINDOW + TITLE_WINDOW_HEIGHT);
    textWindow.setAbsoluteHorizontalPosition(LEFT_CORNER_TITLE_WINDOW);
    textWindow.setRelativeWidth(RelativeWidth::ToRightCorner, windowNum, Relation::Screen);
    textWindow.setHeight(WINDOW_SIZE - TITLE_WINDOW_HEIGHT);
    textWindow.setFont(FONT_NUMBER);
    textWindow.setStartState(StartState::Appears);
    textWindow.setTextAlignment(Window::TextVAlignment::Center, Window::TextHAlignment::Center);
    textWindow.setShowAndClearEffect(Effect::Opacity, Effect::StaticText);
    textWindow.setText(info.text);
    textWindow.setAutoScrolling(0);
    textWindow.setNumberOfRepeat(1);
    textWindow.setDuration(info.showTime);
    textWindow.setShowAndClearSpeed(255, 255);

    result.append(textWindow.getData());

    return result;
}





