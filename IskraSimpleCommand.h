#pragma once

//Qt
#include <QObject>
#include <QScopedPointer>
#include <QList>

//Local
#include "IskraCommandCommon.h"
#include "RouteInfoCreator.h"
#include "RoutePositionCreator.h"

using controlPackets = QList<QByteArray>;

class IskraSimpleCommandPrivate;

//!
//! \brief The IskraSimpleCommand - класс, который позволяет отослать на табло простую команду
//! Которая не требует каких либо изощрений
//!
class IskraSimpleCommand : public QObject
{
    Q_OBJECT
public:
    explicit IskraSimpleCommand(quint8 ledAddress, QObject *parent = nullptr);
    virtual ~IskraSimpleCommand();

    controlPackets packetGetLedInfo();
    controlPackets packetUploadPanelSettings(const IskraCommon::PanelSettings &settings);
    controlPackets packetSetTime();
    controlPackets packetDeleteAllWindow();
    controlPackets packetDeleteSelectWindow(const QList<qint8> &numberOfWindow);
    controlPackets packetUploadBgnImg(const QString& fileName);
    controlPackets packetUploadBgnImg(const QByteArray& data);
    controlPackets packetLoadRouteInfo(const Language language,
                                       const RouteSign routeSign,
                                       const IskraCommon::RouteInfo& routeInfo);
    controlPackets packetLoadRoutePosition(const qint8 currentStationIndex, const State state);
    controlPackets packetLoadDateRepresentationText(const QList<QStringList>& tables);
    controlPackets packetLoadImageLibrary(const QList<QVariant>& images);
    controlPackets packetLoadBrightnessTable();
private:
    quint8  m_ledAddress;
};


