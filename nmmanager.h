#ifndef NMMANAGER_H
#define NMMANAGER_H

#include "networkmodel.h"

class NmManager : public QObject {
    Q_OBJECT
public:
    explicit NmManager(QObject *parent = nullptr);

  // Q_INVOKABLE void onRequestCreateConnection(int connectionType);
  Q_INVOKABLE void onRequestCreateConnection(const QVariantMap &settings);
  // Q_INVOKABLE void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);
  // Q_INVOKABLE void onSelectedConnectionChanged(const QString &connectionPath);
  Q_INVOKABLE void onSelectedConnectionChanged(const QString &connectionUuid);

  void addConnection(const NetworkManager::ConnectionSettings::Ptr &settings);
  void loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &settings);

  /**
   * Removes given connection
   * @connection - d-bus path of the connection you want to edit
   */
  Q_INVOKABLE void removeConnection(const QString &uuid);

public Q_SLOTS:
    /**
     * Activates given connection
     * @connection - d-bus path of the connection you want to activate
     * @device - d-bus path of the device where the connection should be activated
     * @specificParameter - d-bus path of the specific object you want to use for this activation, i.e access point
     */
    void activateConnection(const QString &connection, const QString &device);
    /**
     * Deactivates given connection
     * @connection - d-bus path of the connection you want to deactivate
     * @device - d-bus path of the connection where the connection is activated
     */
    void deactivateConnection(const QString &connection, const QString &device);


Q_SIGNALS:
    void connectionLoaded(const QVariantMap &settings);

private:
    NetworkModel *m_networkModel;
    // QString m_currentConnectionPath;
    QString m_currentConnectionPath;
    NetworkManager::Connection::Ptr connectionFromArgs(const QVariantList &args) const;
    QList<NetworkManager::Connection::Ptr> m_connections;
};

#endif
