#ifndef ROUTEINFOIMAGETEMPLATE_H
#define ROUTEINFOIMAGETEMPLATE_H

#include "AbstractWindowTemplate.h"

/*!
 * \brief The RouteInfoImageTemplate class - класс для формирования окна под изображение, на котором
 * будет отображена нарисованная нами информация о маршруте
 */
class RouteInfoImageTemplate : public AbstractWindowTemplate
{
public:
    explicit RouteInfoImageTemplate(quint8 ledAddress, QObject* parent = nullptr);
protected:
    QList<QByteArray> buildData() override;
private:
    QByteArray createWindowForImage();
};

#endif // ROUTEINFOIMAGETEMPLATE_H
