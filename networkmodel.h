#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QQueue>

// Actually NetworkManagerQt header
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>

// Actually QDbus header
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QPointer>
#include <QTimer>

// ??
// #include <QCoroCore>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>

#include "networkitemslist.h"

class NetworkModel : public QAbstractListModel
{
    Q_OBJECT

public:
    Q_PROPERTY(bool delayModelUpdates READ delayModelUpdates NOTIFY delayModelUpdatesChanged)

    explicit NetworkModel(QObject *parent = nullptr);

    enum ConnectionRoles {
        NameRole = Qt::UserRole + 1,
        UuidRole,
        ConnectionPathRole,
        TypeRole,
        UniRole,
        ItemUniqueNameRole,
        ItemTypeRole,
        statusChanged,
        ConnectionDetailsRole
    };

    enum ModelChangeType {
        ItemAdded,
        ItemRemoved,
        ItemPropertyChanged
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE QVariantMap getConnectionDetails(const QString &uuid);

    // new functions


    QHash<int, QByteArray> roleNames() const override;

    bool delayModelUpdates() const;

Q_SIGNALS:
    void delayModelUpdatesChanged();
    void connectionsChanged();

public slots:
    /**
     * Adds a new connection
     * @map - NMVariantMapMap with connection settings
     */
    void addConnectionFromMap(const NMVariantMapMap &map);
    /**
     * Removes given connection
     * @connection - d-bus path of the connection you want to edit
     */
    void removeConnection(const QString &connection);
    /**
     * Updates given connection
     * @connection - connection which should be updated
     * @map - NMVariantMapMap with new connection settings
     */
    void updateConnection(const QString &uuid, const QVariantMap &settings);
    // void updateConnection(NetworkManager::Connection::Ptr connection, const NMVariantMapMap &map);

    // load details, initialize, type, ipv4 widget
    void setConnection(const NetworkManager::ConnectionSettings::Ptr &connection);



private Q_SLOTS:
    void initialize();
    void connectionUpdated();
    void connectionAdded(const QString &connection);
    void connectionRemoved(const QString &connection);

private:
    bool m_delayModelUpdates = false;
    NetworkItemsList m_list;
    QQueue<QPair<ModelChangeType, NetworkModelItem *>> m_updateQueue;

    // void addConnection(NMConnection *connection);
    void addConnection(const NetworkManager::Connection::Ptr &connection);
    void initializeSignals();
    void initializeSignals(const NetworkManager::Connection::Ptr &connection);
    void removeConnectionInternal(const QString &connection);
    QString m_tmpConnectionPath;
    QString m_tmpConnectionUuid;
    QString m_tmpDevicePath;
    // QString m_tmpSpecificPath;

    // void handleConnectionAdded(const QString &path);
    // void handleConnectionRemoved((const QString &path);
    void flushUpdateQueue();
    void updateDelayModelUpdates();

    void insertItem(NetworkModelItem *item);
    void removeItem(NetworkModelItem *item);
    void updateItem(NetworkModelItem *item);


    void insertItem(int index);
    void removeItem(int index);
    // void updateItem(const QModelIndex &index, const QVariant &value, int role);

    NetworkManager::Connection::Ptr connectionFromArgs(const QVariantList &args) const;

    QList<NetworkManager::Connection::Ptr> m_connections;
};

#endif // NETWORKMODEL_H
