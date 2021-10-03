#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "hcucontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("viktorgino");
    app.setOrganizationDomain("viktorgino.me");
    app.setApplicationName("HCU Client");
    QQmlApplicationEngine engine;

    HCUController controller(&engine);
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    engine.rootContext()->setContextProperty("HCUConnection",&controller);
    engine.load(url);
    app.exec();
    return 1;
}
