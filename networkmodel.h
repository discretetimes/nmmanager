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

class NetworkModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ConnectionRoles {
        NameRole = Qt::UserRole + 1,
        UuidRole,
        TypeRole
    };

    explicit NetworkModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // new functions
    void refresh();
    void addConnection(NMConnection *connection);

    QHash<int, QByteArray> roleNames() const override;

public slots:
    /**
     * Adds a new connection
     * @map - NMVariantMapMap with connection settings
     */
    void addConnection(const NMVariantMapMap &map);
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
    void updateConnection(NetworkManager::Connection::Ptr connection, const NMVariantMapMap &map);

    // load details, initialize, type, ipv4 widget
    void setConnection(const NetworkManager::ConnectionSettings::Ptr &connection);

Q_SIGNALS:
    void connectionsChanged();

private:
    void removeConnectionInternal(const QString &connection);
    QString m_tmpConnectionPath;
    QString m_tmpConnectionUuid;
    QString m_tmpDevicePath;
    // QString m_tmpSpecificPath;

    QList<NetworkManager::Connection::Ptr> m_connections;
};

#endif // NETWORKMODEL_H
