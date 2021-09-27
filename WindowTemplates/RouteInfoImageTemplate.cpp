#include "RouteInfoImageTemplate.h"
#include <QDebug>

using namespace ISKRA;
using namespace Window;

namespace
{
    const int WINDOW_NUM = 1;
    const Window::Color WHITE       = Window::Color(255, 255, 255, 255);
    const Window::Color TRANSPARENT = Window::Color(0, 0, 0, 0);
}

RouteInfoImageTemplate::RouteInfoImageTemplate(quint8 ledAddress, QObject *parent)
    : AbstractWindowTemplate(ledAddress, parent)
{}

QList<QByteArray> RouteInfoImageTemplate::buildData()
{
    qDebug() << Q_FUNC_INFO << "Led adresss" << m_ledAddress;

    QList<QByteArray> data;

    data.append(Packet(m_ledAddress, 0x80, Command::CreateTextWindow, createWindowForImage()).encode());

    return data;
}

QByteArray RouteInfoImageTemplate::createWindowForImage()
{
    WindowCreator creator;
    creator.setNumber(WINDOW_NUM);

    creator.setColor(TRANSPARENT, WHITE);
    creator.setVerticalPosition(0);
    creator.setAbsoluteHorizontalPosition(0);
    creator.setAbsoluteWidth(292);
    creator.setHeight(128);
    //TODO: добавить в конструктор
    creator.setFont(4);
    creator.setStartState(StartState::Appears);
    creator.setText(InlineCommands::imageWord("01"));
    creator.setTextAlignment(TextVAlignment::Top, TextHAlignment::Left);
    creator.setNumberOfRepeat(0);
    creator.setDuration(0);
    creator.setShowAndClearSpeed(8, 8);
    creator.setAutoScrolling(0);

    return creator.getData();
}
