#include <QCoreApplication>

#include "MainController.h"

Q_DECLARE_METATYPE(CurrentTableInfo)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<QMqttTopicFilter>();
    qRegisterMetaType<QMqttMessage>();
    qRegisterMetaType<CurrentTableInfo>();

    MainController controller;

    return a.exec();
}
