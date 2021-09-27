#pragma once

#include "AbstractWindowTemplate.h"
#include "Global.h"

#include <QObject>

using namespace Global::;

class RouteInfoTemplate : public AbstractWindowTemplate
{
    Q_OBJECT
public:
    explicit RouteInfoTemplate(quint8 ledAddress, QObject *parent = nullptr);
    ~RouteInfoTemplate() override;
    /*!
     * \brief addRouteInfo - добавляем маршрутную информациб
     * \return возвращает номер окна под время прибытия
     */
    int addRouteInfo(const ArriveRouteInfo& info);
    void reset() override;
    QByteArray updateArriveTime(int min, int windowNum) override;
private:
    QList<QByteArray> buildData() override;
    QByteArray createBgnWindow();
    QByteArray createTitleWindow(const QString& titleText);
    QByteArray createTimeWindow();
    QByteArray createTemperatureWindow(int temp);
    QByteArray createMinTitleWindow();
    QByteArray createRouteNumWindow(const QString& routeNum, const Window::Color textColor);
    QByteArray createRouteInfoWindow(const QString& routeInfo, Window::Color textColor, int num = -1);
    QByteArray createArriveTimeWindow(int timeToArrive);
    QByteArray updateArriveTimeWindow(int min, int windowNum);
    QByteArray createRouteIcon(const QString &imageNumber);
    QByteArray createBtnINFO();
private:
    QList<ArriveRouteInfo>  m_routesInfo;
};

