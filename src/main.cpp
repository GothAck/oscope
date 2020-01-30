#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "scope.hpp"
#include "customvideosurface.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("GothAck");
    QCoreApplication::setOrganizationDomain("gothack.ninja");
    QCoreApplication::setApplicationName("OScope");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Scope>("oscope", 1, 0, "Scope");
    qmlRegisterType<CustomVideoSurface>("oscope", 1, 0, "CustomVideoSurface");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
