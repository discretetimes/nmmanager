#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "nmmanager.h"
#include "networkmodel.h"
#include "connectionproxymodel.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  NmManager nmManager;
  NetworkModel networkModel;
  ConnectionProxyModel connectionProxyModel;
  engine.rootContext()->setContextProperty("nmManager", &nmManager);
  engine.rootContext()->setContextProperty("networkModel", &networkModel);
  engine.rootContext()->setContextProperty("connectionProxyModel", &connectionProxyModel);
  engine.load(QUrl("qrc:/main.qml"));
  return app.exec();
}
