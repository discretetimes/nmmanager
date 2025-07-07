#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "nmmanager.h"
#include "networkmodel.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  NmManager nmManager;
  NetworkModel networkModel;
  engine.rootContext()->setContextProperty("nmManager", &nmManager);
  engine.rootContext()->setContextProperty("networkModel", &networkModel);
  engine.load(QUrl("qrc:/main.qml"));
  return app.exec();
}
