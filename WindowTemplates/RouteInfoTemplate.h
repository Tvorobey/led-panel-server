#pragma once

#include "AbstractWindowTemplate.h"
#include "Global.h"

#include <QObject>

using namespace Global::Defines;
using namespace Global::Colors;
using namespace Global::Fonts;

/*!
 * \brief The RouteInfoTemplate - класс, который занимается созданием текстовых окон для
 * отображения основной информации на остановке, такой как: название остановки, подъезжающие ТС,
 * дата, время и тд...
 */
class RouteInfoTemplate : public AbstractWindowTemplate
{
    Q_OBJECT
public:
    explicit RouteInfoTemplate(quint8 ledAddress, QObject *parent = nullptr);
    ~RouteInfoTemplate() override;
    /*!
     * \brief getStationNameData - возвращает окно под остановку
     * \param name - имя остановки
     */
    QList<QByteArray>   getStationNameWindow(const QString& name, int windowNum);
    /*!
     * \brief getAdditionalWindow - возвращает окна, под время, дату, и тд
     * \param firstWindowNum - номер первого окна      * \return
     */
    QList<QByteArray>   getAdditionalWindow(int firstWindowNum, bool showTemp = false, int temperature = 0,
                                            Window::Color tempColor = WHITE);
    /*!
     * \brief getArrivingVehicleWindow - создает окно под ТС, которое прибывает
     * \param info - информация о ТС
     * \param firstWindowNum - номер первого окна
     * \param numOfCreatedWindow - количество раз, сколько раз была создана инфа о прибывающих ТС
     * \return
     */
    QList<QByteArray>   getArrivingVehicleWindow(const VehicleInfo& info, int firstWindowNum, int numOfCreatedWindow);
    void reset() override;
private:
    QList<QByteArray> buildData() override {}
    QByteArray createTitleWindow(const QString& titleText, int windowNum);
    QList<QByteArray> createAdditionalInfoWindow(int firstWindowNum, bool showTemp = false, int temperature = 0,
                                                 Window::Color tempColor = WHITE);
    QList<QByteArray> createArrivingVehicleWindow(const VehicleInfo& info, int firstWindowNum, int numOfCreatedWindow);
private:
    StationInfo             m_stationInfo;
};

