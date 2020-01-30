#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "scope.hpp"
#include "discovery.hpp"
#include "customvideosurface.hpp"
#include "vendor/IconFontCppHeaders/IconsFontAwesome5.h"

const QString icon_fa_play(ICON_FA_PLAY);
const QString icon_fa_stop(ICON_FA_STOP);
const QString icon_fa_home(ICON_FA_HOME);
const QString icon_fa_eject(ICON_FA_EJECT);

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("GothAck");
    QCoreApplication::setOrganizationDomain("gothack.ninja");
    QCoreApplication::setApplicationName("OScope");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Scope>("oscope", 1, 0, "Scope");
    qmlRegisterUncreatableType<Discovery>("oscope", 1, 0, "Discovery", "C++ only");
    qmlRegisterUncreatableType<DiscoveredNode>("oscope", 1, 0, "DiscoveredNode", "C++ only");
    qmlRegisterType<CustomVideoSurface>("oscope", 1, 0, "CustomVideoSurface");

    auto discovery = new Discovery(&app);
    discovery->start();
    engine.rootContext()->setContextProperty("discovery", discovery);
    engine.rootContext()->setContextProperty("ICON_FA_PLAY", QVariant::fromValue(icon_fa_play));
    engine.rootContext()->setContextProperty("ICON_FA_STOP", QVariant::fromValue(icon_fa_stop));
    engine.rootContext()->setContextProperty("ICON_FA_HOME", QVariant::fromValue(icon_fa_home));
    engine.rootContext()->setContextProperty("ICON_FA_EJECT", QVariant::fromValue(icon_fa_eject));

    QFontDatabase::addApplicationFont("qrc:/fontawesome5solid.otf");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
