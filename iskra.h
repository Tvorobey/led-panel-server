#ifndef ISKRA_H
#define ISKRA_H


#include <QString>

namespace ISKRA {

enum class Command
{
    GetFirmawareInfo                        = 0x00,
    // TODO: FWDoPrepare,
    // TDOD: FWWrite,
    GetSettings                             = 0x03,
    LoadSettings                            = 0x04,
    SetTime,
    GetTime,
    LoadDateRepresentationTextTable         = 0x07,
    GetDateRepresentationTextTable          = 0x08,
    LoadBrightnessAdjustmentsTable          = 0x09,
    GetFontTable                            = 0x10,
    ClearFontTable                          = 0x11,
    LoadFont                                = 0x13,
    GetImageLibraryInfo                     = 0x14,
    LoadImageLibrary                        = 0x15,
    DeleteAllWindow                         = 0x20,
    CreateTextWindow                        = 0x21,
    DeleteListOfWindow                      = 0x22,
    UploadBgnImage                          = 0x23,
    CheckCurrentWindowState                 = 0x24,
    UploadRouteInfo                         = 0x31,
    UploadRoutePosition                     = 0x32,
    GetFirmwareInfoResponce                 = 0x80,
    GetSettingsResponse,
    LoadSettingsResponse                    = 0x84,
    SetTimeResponse,
    GetTimeResponse,
    LoadDateRepresentationTextTableResponse = 0x87,
    GetDateRepresentationTextTableResponse  = 0x88,
    LoadBrightnessAdjustmentsTableResponse  = 0x89,
    GetFontTableResponse                    = 0x90,
    ClearFontTableResponse                  = 0x91,
    LoadFontResponse                        = 0x93,
    GetImageLibraryInfoResponse             = 0x94,
    LoadImageLibraryResponse                = 0x95,
    DeleteAllWindowResponse                 = 0xA0,
    CreateTextWindowResponse                = 0xA1,
    DeleteListOfWindowResponse              = 0xA2,
    UploadBgnImageResponse                  = 0xA3,
    CheckCurrentWindowStateResponse         = 0xA4,
    UploadRouteInfoResponse                 = 0xB1,
    UploadRoutePositionResponse             = 0xB2,
    Unknown                                 = 0xFF
};



inline QString toString(Command c)
{
    switch (c)
    {
        case Command::GetFirmawareInfo:  return "GetFirmawareInfo";
        case Command::GetSettings:       return "GetSettings";
        case Command::GetFirmwareInfoResponce: return "GetFirmwareInfoResponse";
        case Command::LoadSettingsResponse: return "LoadSettingsResponse";
        case Command::SetTimeResponse: return "SetTimeResponse";
        case Command::GetTimeResponse: return "GetTimeResponse";
        case Command::LoadDateRepresentationTextTableResponse: return "LoadDateRepresentationTextTableResponse";
        case Command::GetDateRepresentationTextTableResponse: return "GetDateRepresentationTextTableResponse";
        case Command::LoadBrightnessAdjustmentsTableResponse: return "LoadBrightnessAdjustmentsTableResponse";
        case Command::GetFontTableResponse: return "GetFontTableResponse";
        case Command::ClearFontTableResponse: return "ClearFontTableResponse";
        case Command::LoadFontResponse: return "LoadFontResponse";
        case Command::GetImageLibraryInfoResponse: return "GetImageLibraryInfoResponse";
        case Command::LoadImageLibraryResponse: return "LoadImageLibraryResponse";
        case Command::DeleteAllWindowResponse: return "DeleteAllWindowResponse";
        case Command::CreateTextWindowResponse: return "CreateTextWindowResponse";
        case Command::DeleteListOfWindowResponse: return "DeleteListOfWindowResponse";
        case Command::UploadBgnImageResponse: return "UploadBgnImageResponse";
        case Command::CheckCurrentWindowStateResponse: return "CheckCurrentWindowStateResponse";
        case Command::UploadRouteInfoResponse: return "UploadRouteInfoResponse";
        case Command::UploadRoutePositionResponse: return "UploadRoutePositionResponse";
        default:
        return "Unknown";
    }
}

enum class CommandResult
{
    OK                    = 0x00,
    FWPrepartionError,
    FWWriteError,
    SettingsWriteError    = 0x04,
    FontTableClearError   = 0x11,
    FontHeaderWriteError,
    FontBodyWriteError,
    ClearWindowsError     = 0x20,
    CreateWindowError,
    DeleteWindowError,
    BackImageLoadError,
    RouteInfoLoadError    = 0x31,
    WrongPacketStruct     = 0x80,
    IncomplePacket,
    WrongCRC,
    WronPAcketLength,
    Unknown               = 0xFF
};

inline QString toString(CommandResult cr)
{
    switch (cr) {
    case CommandResult::OK:                   return "OK";
    case CommandResult::FWPrepartionError:    return "FWPrepartionError";
    case CommandResult::FWWriteError:         return "FWWriteError";
    case CommandResult::SettingsWriteError:   return "SettingsWriteError";
    case CommandResult::FontTableClearError : return "FontTableClearError";
    case CommandResult::FontHeaderWriteError: return "FontHeaderWriteError";
    case CommandResult::FontBodyWriteError:   return "FontBodyWriteError";
    case CommandResult::ClearWindowsError:    return "ClearWindowsError";
    case CommandResult::CreateWindowError:    return "CreateWindowError";
    case CommandResult::DeleteWindowError:    return "DeleteWindowError";
    case CommandResult::BackImageLoadError:   return "BackImageLoadError";
    case CommandResult::RouteInfoLoadError:   return "RouteInfoLoadError";
    case CommandResult::WrongPacketStruct:    return "WrongPacketStruct";
    case CommandResult::IncomplePacket:       return "IncomplePacket";
    case CommandResult::WrongCRC:             return "WrongCRC";
    case CommandResult::WronPAcketLength:     return "WronPAcketLength";
    default:
         return "Unknown";
    }
}


/*
 *  class Packet
 */
class Packet
{
    static const uint8_t    PckgSep1;
    static const uint8_t    PckgSep2;
    static const uint8_t    PckgPrefixEnd;
    static const uint8_t    PckgSuffixEnd;
    static const uint8_t    PckgDummyMiddle;
    static const uint8_t    PckgDummyEnd;
    static const QByteArray PckgPrefix;
    static const QByteArray PckgSuffix;

    static QByteArray makePrefix();
    static QByteArray makeSuffix();
    static uint32_t   crc32(const QByteArray& );
    static QByteArray encode(const QByteArray& );

public:
    Packet();
    Packet(uint8_t receiver, uint8_t sender, Command cmd, const QByteArray& data);
    Packet(const QByteArray& inPacket);

    uint8_t           getReceiver() const;
    uint8_t           getSender() const;
    Command           getCommand() const;
    CommandResult     getCommandResult() const;
    const QByteArray& getData() const;

    QByteArray encode() const;
private:
    void decode(const QByteArray& packet);

private:
    uint8_t       m_Receiver;
    uint8_t       m_Sender;
    Command       m_Command;
    CommandResult m_CommandResult;
    QByteArray    m_Data;
};

inline Packet::Packet() : m_Receiver(0), m_Sender(0), m_Command(Command::Unknown), m_CommandResult(CommandResult::Unknown )
{
}

inline Packet::Packet(uint8_t receiver, uint8_t sender, Command cmd, const QByteArray& data) :
    m_Receiver(receiver), m_Sender(sender), m_Command(cmd), m_CommandResult(CommandResult::OK), m_Data(data)
{

}

inline Packet::Packet(const QByteArray& inPacket) : m_Receiver(0xFF),m_Sender(0xFF),m_Command(Command::GetFirmawareInfo),m_CommandResult(CommandResult::Unknown)
{
    decode(inPacket);
}

inline uint8_t Packet::getReceiver() const
{
    return m_Receiver;
}

inline uint8_t Packet::getSender() const
{
    return m_Sender;
}

inline Command Packet::getCommand() const
{
    return  m_Command;
}

inline CommandResult Packet::getCommandResult() const
{
    return  m_CommandResult;
}

inline const QByteArray& Packet::getData() const
{
    return m_Data;
}

}
#endif // ISKRA_H
