#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "nmmanager.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  NmManager nmManager;
  engine.rootContext()->setContextProperty("nmManager", &nmManager);
  engine.load(QUrl("qrc:/main.qml"));
  return app.exec();
}
