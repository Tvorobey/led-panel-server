//Local
#include "IskraSimpleCommand.h"
#include "iskra.h"
#include "panelsettingcreator.h"
#include "TimeSetter.h"
#include "BrightnessAdjustmentRableCreator.h"
#include "ListOfWindowDeleter.h"
#include "BgnWindowCreator.h"
#include "DateRepresentationTextCreator.h"
#include "ImageLibraryCreator.h"

using namespace ISKRA;

IskraSimpleCommand::IskraSimpleCommand(quint8 ledAddress, QObject *parent)
    : QObject(parent),
      m_ledAddress(ledAddress)
{}

IskraSimpleCommand::~IskraSimpleCommand()
{}

controlPackets IskraSimpleCommand::packetGetLedInfo()
{
    controlPackets packets;

    packets.append(Packet(m_ledAddress, 0x80, Command::GetSettings, QByteArray()).encode());

    return packets;
}

controlPackets IskraSimpleCommand::packetUploadPanelSettings(const IskraCommon::PanelSettings& settings)
{
    PanelSettingCreator creator(settings);

    controlPackets packets;
    packets.append(Packet(m_ledAddress, 0x80, Command::LoadSettings, creator.getData()).encode());

    return packets;
}

controlPackets IskraSimpleCommand::packetSetTime()
{
    return controlPackets({Packet(m_ledAddress, 0x80, Command::SetTime, TimeSetter::getData()).encode()});
}

controlPackets IskraSimpleCommand::packetDeleteAllWindow()
{
    return controlPackets({Packet(m_ledAddress, 0x80, Command::DeleteAllWindow, QByteArray()).encode()});
}

controlPackets IskraSimpleCommand::packetDeleteSelectWindow(const QList<qint8>& numberOfWindow)
{
    ListOfWindowDeleter deleter(numberOfWindow);

    return controlPackets({Packet(m_ledAddress, 0x80, Command::DeleteListOfWindow, deleter.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetUploadBgnImg(const QString &fileName)
{
    BgnWindowCreator creator;
    creator.setImage(fileName);

    return controlPackets({Packet(m_ledAddress, 0x80, Command::UploadBgnImage, creator.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetUploadBgnImg(const QByteArray &data)
{
    BgnWindowCreator creator;
    creator.setImage(data);

    return controlPackets({Packet(m_ledAddress, 0x80, Command::UploadBgnImage, creator.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetLoadRouteInfo(const Language language, const RouteSign routeSign,
                                                 const RouteInfo &routeInfo)
{
    RouteInfoCreator creator;

    creator.setLanguageAndRouteSign(language, routeSign);
    creator.setRouteNumber(routeInfo.routeNumber);
    creator.setStartStationName(routeInfo.startStationName);
    creator.setEndStationName(routeInfo.endStationName);
    creator.setStationsList(routeInfo.stationsList);

    return controlPackets({Packet(m_ledAddress, 0x80, Command::UploadRouteInfo, creator.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetLoadRoutePosition(const qint8 currentStationIndex, const State state)
{
    return controlPackets({Packet(m_ledAddress, 0x80, Command::UploadRouteInfo,
                           RoutePositionCreator::getData(currentStationIndex, state)).encode()});
}

controlPackets IskraSimpleCommand::packetLoadDateRepresentationText(const QList<QStringList> &tables)
{
    DateRepresentationTextCreator creator;

    for (const auto& table : tables)
        creator.appendDateTable(table);

    return controlPackets({Packet(m_ledAddress, 0x80, Command::LoadDateRepresentationTextTable,
                           creator.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetLoadImageLibrary(const QList<QVariant> &images)
{
    ImageLibraryCreator creator;

    for (const auto& data : images)
    {
        if ((data.toString().split(".").last() == "bmp") || (data.toString().split(".").last() == "png"))
            creator.appendImage(data.toString());
        else
            creator.appendImage(data.toByteArray());
    }

    return controlPackets({Packet(m_ledAddress, 0x80, Command::LoadImageLibrary, creator.getData()).encode()});
}

controlPackets IskraSimpleCommand::packetLoadBrightnessTable()
{
    BrightnessAdjustmentTableCreator creator;

    return controlPackets({Packet(m_ledAddress, 0x80, Command::LoadBrightnessAdjustmentsTable, creator.getData()).encode()});
}


