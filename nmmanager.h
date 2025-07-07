#ifndef NMMANAGER_H
#define NMMANAGER_H

#include "networkmodel.h"

class NmManager : public QObject {
    Q_OBJECT
public:
    explicit NmManager(QObject *parent = nullptr);

  Q_INVOKABLE void onRequestCreateConnection(int connectionType);
  // Q_INVOKABLE void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);
  Q_INVOKABLE void onSelectedConnectionChanged(const QString &connectionPath);

  void addConnection(const NetworkManager::ConnectionSettings::Ptr &settings);
  void loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &settings);

Q_SIGNALS:
    void connectionLoaded(const QVariantMap &settings);

private:
    NetworkModel *m_networkModel;
    // QString m_currentConnectionPath;
    QString m_currentConnectionPath;

    NetworkManager::Connection::Ptr connectionFromArgs(const QVariantList &args) const;
};

#endif
