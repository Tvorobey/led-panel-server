QT += gui network mqtt

CONFIG += c++11 console
CONFIG -= app_bundle

unix {
    OBJECTS_DIR = .obj
    MOC_DIR     = .moc

    INSTALLS += target
    target.path=/usr/bin

    QMAKE_CLEAN += $(TARGET) $(QMAKE_TARGET)
#    QMAKE_CXXFLAGS += -Wall -Werror -Wextra -pedantic
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += Command/ \
               WindowTemplates/

SOURCES += \
        Command/BgnWindowCreator.cpp \
	Command/BrightnessAdjustmentRableCreator.cpp \
	Command/DateRepresentationTextCreator.cpp \
	Command/ImageLibraryCreator.cpp \
	Command/ListOfWindowDeleter.cpp \
	Command/panelsettingcreator.cpp \
	Command/RouteInfoCreator.cpp \
	Command/RoutePositionCreator.cpp \
	Command/TimeSetter.cpp \
	Command/WindowCreator.cpp \
	IskraSimpleCommand.cpp \
	IskraTcpServer.cpp \
	LedController.cpp \
	MainController.cpp \
	MqttClientHandler.cpp \
    Parsers/CurrentTableInfoTopic.cpp \
    Parsers/EmergencyTopicParser.cpp \
	Parsers/LedDiagnosticTopic.cpp \
	Parsers/TopicRouteInfoParser.cpp \
	Parsers/WeatherTopicParser.cpp \
    WindowTemplates/EmergencyWindowTemplate.cpp \
	WindowTemplates/RouteInfoTemplate.cpp \
	SocketThread.cpp \
	iskra.cpp \
	WindowTemplates/AbstractWindowTemplate.cpp \
	main.cpp

HEADERS += \
        Command/BgnWindowCreator.h \
	Command/BrightnessAdjustmentRableCreator.h \
	Command/DateRepresentationTextCreator.h \
	Command/ImageLibraryCreator.h \
	Command/ListOfWindowDeleter.h \
	Command/panelsettingcreator.h \
	Command/RouteInfoCreator.h \
	Command/RoutePositionCreator.h \
	Command/TimeSetter.h \
	Command/WindowCreator.h \
	Global.h \
	IskraCommandCommon.h \
	IskraSimpleCommand.h \
	LedController.h \
	MainController.h \
	MqttClientHandler.h \
    Parsers/CurrentTableInfoTopic.h \
    Parsers/EmergencyTopicParser.h \
	Parsers/LedDiagnosticTopic.h \
	Parsers/TopicRouteInfoParser.h \
	Parsers/WeatherTopicParser.h \
    WindowTemplates/EmergencyWindowTemplate.h \
	WindowTemplates/RouteInfoTemplate.h \
	SocketThread.h \
	iskra.h \
	WindowTemplates/AbstractWindowTemplate.h \
	IskraTcpServer.h

RESOURCES +=





