#include "networkmodel.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredSetting>

#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QFile>

using namespace NetworkManager;

NetworkModel::NetworkModel(QObject *parent)
    : QAbstractListModel(parent)
{
    refresh();
}

int NetworkModel::rowCount(const QModelIndex &parent) const
{
    // For list models, the row count is the size of the list.
    if (parent.isValid())
        return 0;

    return m_connections.count();
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const NetworkManager::Connection::Ptr &connection = m_connections.at(index.row());

    switch (role) {
    case NameRole:
        return connection->name();
    case UuidRole:
        return connection->uuid();
    case ConnectionPathRole:
        return connection->path();
    case TypeRole:
        return connection->settings()->connectionType();
    }

    return QVariant();
}

bool NetworkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // Here you would implement logic to update the connection using NetworkManager
        // For example:
        // m_connections[index.row()]->settings()->setId(value.toString());
        // m_connections[index.row()]->update(m_connections[index.row()]->settings()->toMap());
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags NetworkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // Or whatever flags you need
}

QHash<int, QByteArray> NetworkModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[UuidRole] = "uuid";
    roles[ConnectionPathRole] = "path";
    roles[TypeRole] = "type";
    return roles;
}

void NetworkModel::refresh()
{
    beginResetModel();
    qInfo() << "refresh";
    m_connections = NetworkManager::listConnections();
    qInfo() << "Wired con count:" << m_connections.count();
    for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
        // insertConnection(connection);
        qInfo() << "Con name:" << connection->name() ;
    }
    endResetModel();
}

struct AddConnectionData {
    QString id;
    NetworkModel *handler;
};

// void add_connection_cb(GObject *client, GAsyncResult *result, gpointer user_data)
// {
//     AddConnectionData *data = static_cast<AddConnectionData *>(user_data);
//
//     GError *error = nullptr;
//     NMRemoteConnection *connection = nm_client_add_connection2_finish(NM_CLIENT(client), result, NULL, &error);
//
//     if (error) {
//         // KNotification *notification = new KNotification(QStringLiteral("FailedToAddConnection"), KNotification::CloseOnTimeout, data->handler);
//         // notification->setTitle(i18n("Failed to add connection %1", data->id));
//         // notification->setComponentName(QStringLiteral("networkmanagement"));
//         // notification->setText(QString::fromUtf8(error->message));
//         // notification->setIconName(QStringLiteral("dialog-warning"));
//         // notification->sendEvent();
//
//         g_error_free(error);
//     } else {
//         // KNotification *notification = new KNotification(QStringLiteral("ConnectionAdded"), KNotification::CloseOnTimeout, data->handler);
//         // notification->setText(i18n("Connection %1 has been added", data->id));
//         // notification->setComponentName(QStringLiteral("networkmanagement"));
//         // notification->setTitle(data->id);
//         // notification->setIconName(QStringLiteral("dialog-information"));
//         // notification->sendEvent();
//
//         g_object_unref(connection);
//     }
//
//     delete data;
// }


void NetworkModel::addConnection(NMConnection *connection)
{
    // NMClient *client = nm_client_new(nullptr, nullptr);
    //
    // AddConnectionData *userData = new AddConnectionData{QString::fromUtf8(nm_connection_get_id(connection)), this};
    //
    // nm_client_add_connection2(client,
    //                           nm_connection_to_dbus(connection, NM_CONNECTION_SERIALIZE_ALL),
    //                           NM_SETTINGS_ADD_CONNECTION2_FLAG_TO_DISK,
    //                           nullptr,
    //                           true,
    //                           nullptr,
    //                           add_connection_cb,
    //                           userData);
    // QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(newSettings->toMap());
}

void NetworkModel::addConnection(const NMVariantMapMap &map)
{
        // NetworkManager::addConnection(map);
        // beginResetModel();
        // m_connections = NetworkManager::listConnections();
        // endResetModel();
    // const QString connectionId = map.value(QStringLiteral("connection")).value(QStringLiteral("id")).toString();
    QDBusReply<QDBusObjectPath> reply = NetworkManager::addConnection(map);
}

void NetworkModel::removeConnection(const QString &connection)
{
        // NetworkManager::Connection::Ptr conn = NetworkManager::findConnection(connection);
        // if (conn) {
        //     conn->remove();
        //     beginResetModel();
        //     m_connections = NetworkManager::listConnections();
        //     endResetModel();
        // }
    removeConnectionInternal(connection);
}

void NetworkModel::removeConnectionInternal(const QString &connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con || con->uuid().isEmpty()) {
        // qCWarning(PLASMA_NM_LIBS_LOG) << "Not possible to remove connection " << connection;
        return;
    }

    // Remove slave connections
    for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == con->uuid()) {
            connection->remove();
        }
    }

    // QPointer<Handler> thisGuard(this);
    QDBusReply<void> reply = con->remove();
    // if (!thisGuard) {
    //     co_return;
    // }

    if (!reply.isValid()) {
        // KNotification *notification = new KNotification(QStringLiteral("FailedToRemoveConnection"), KNotification::CloseOnTimeout, this);
        // notification->setTitle(i18n("Failed to remove %1", con->name()));
        // notification->setComponentName(QStringLiteral("networkmanagement"));
        // notification->setText(reply.error().message());
        // notification->setIconName(QStringLiteral("dialog-warning"));
        // notification->sendEvent();
    } else {
        // KNotification *notification = new KNotification(QStringLiteral("ConnectionRemoved"), KNotification::CloseOnTimeout, this);
        // notification->setText(i18n("Connection %1 has been removed", con->name()));
        // notification->setComponentName(QStringLiteral("networkmanagement"));
        // notification->setTitle(con->name());
        // notification->setIconName(QStringLiteral("dialog-information"));
        // notification->sendEvent();
    }
}

// void NetworkModel::updateConnection(const QString &connectionUuid, const QVariantMap &map)
// {
//         // connection->update(map);
//     QVariantList args;
//     args << "Uuid=" + connectionUuid;
//     NetworkManager::Connection::Ptr connection = connectionFromArgs(args);
//     QDBusReply<void> reply = connection->update(map);
// }

void NetworkModel::updateConnection(const QString &uuid, const QVariantMap &settings)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);
    if (!connection) {
        qWarning() << "Could not find connection with UUID:" << uuid;
        return;
    }

    // obtain settings from existing
    ConnectionSettings::Ptr m_settings = connection->settings();
    Ipv4Setting::Ptr m_ipv4setting = m_settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();

    // 1. Build the 'connection' settings map
    m_settings->setId("test-1");
    // QVariantMap connectionSettings;
    // connectionSettings["id"] = m_settings->id();
    // connectionSettings["uuid"] = m_settings->uuid(); // Use the existing UUID
    // connectionSettings["type"] = "802-3-ethernet";
    // newSettingsMap["connection"] = connectionSettings;

    // 2. Build the '802-3-ethernet' (wired) settings map
    // QVariantMap wiredSettings;
    // wiredSettings["device"] = settings["device"].toString();
    // newSettingsMap["802-3-ethernet"] = wiredSettings;

    // 3. Build the 'ipv4' settings map
    // ("ipv4", QMap(("address-data", QVariant(NMVariantMapList, ))("addresses", QVariant(UIntListList, ))("method", QVariant(QString, "manual"))))
    // 1. Update IPv4 settings -------------------------------------------------
    // QVariantMap ipv4Map = newSettingsMap.contains("ipv4") ? newSettingsMap["ipv4"].toMap() : QVariantMap();
    if (settings.contains("ipv4Method")) {
        int method = settings["ipv4Method"].toInt();
        if(method == 0)
            m_ipv4setting->setMethod(Ipv4Setting::Automatic);
        else
            m_ipv4setting->setMethod(Ipv4Setting::Manual);
    }

    if (settings.contains("addresses")) {
        QList<IpAddress> addresses;
        for(const QVariant &addrVariant : settings["addresses"].toList()) {
            QStringList parts = addrVariant.toString().split('/');
            if(parts.length() == 2) {
                IpAddress address;
                address.setIp(QHostAddress(parts[0]));
                address.setPrefixLength(parts[1].toUInt());
                addresses.append(address);
            }
        }
        m_ipv4setting->setAddresses(addresses);
    }

    QVariantMap newipv4setting = m_ipv4setting->toMap();
    NMVariantMapMap newSettingsMap = m_settings->toMap();
    qInfo() << "Update settings: " << newSettingsMap;
    QDBusReply<void> reply = connection->update(newSettingsMap);
    qInfo() << "Update settings: " << reply.error().message();
    // m_networkModel->updateConnection(connection, newSettingsMap);
}

void NetworkModel::setConnection(const NetworkManager::ConnectionSettings::Ptr& connection)
{
    QVariantMap details;
    details["name"] = connection->name();
    NetworkManager::WiredSetting::Ptr wiredSetting = connection->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
    // if (wiredSetting) {
    //     details["device"] = wiredSetting->device();
    // }

    NetworkManager::Ipv4Setting::Ptr ipv4Setting = connection->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
    if (ipv4Setting) {
        QVariantMap ipv4Details;
        ipv4Details["method"] = ipv4Setting->method();
        // You would need to properly handle addresses, as they are a list of Address objects
        // This is a simplified example
        // if (!ipv4Setting->addresses().isEmpty()) {
        //     ipv4Details["address"] = ipv4Setting->addresses().first().address();
        // }
        // details["ipv4"] = ipv4Details;
    }
    // NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
    NMVariantMapMap allSettings = connection->toMap();
    qInfo() << "Connection details: " <<  allSettings;
}

QVariantMap NetworkModel::get(int row)
{
    if (row < 0 || row >= m_connections.count())
        return QVariantMap();

    QVariantMap res;
    const auto &connection = m_connections.at(row);
    res["name"] = connection->name();
    res["uuid"] = connection->uuid();
    return res;
}

QVariantMap NetworkModel::getConnectionDetails(const QString &uuid)
{
    QVariantMap details;
    for (const auto &conn : m_connections) {
        if (conn->uuid() == uuid) {
            details["name"] = conn->name();
            details["uuid"] = conn->uuid();
            details["type"] = conn->settings()->connectionType();

            // Add more details as needed
            Ipv4Setting::Ptr ipv4Setting = conn->settings()->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
            if (ipv4Setting) {
                details["ipv4Method"] = ipv4Setting->method();
                qInfo() << "Method" << ipv4Setting->method();
                QList<IpAddress> addresses = ipv4Setting->addresses();
                QStringList addrStrings;
                for(const auto& addr : addresses) {
                    addrStrings.append(addr.ip().toString() + "/" + QString::number(addr.prefixLength()));
                }
                details["addresses"] = addrStrings;
            }
            return details;
        }
    }
    return details; // Return empty map if not found
}

NetworkManager::Connection::Ptr NetworkModel::connectionFromArgs (const QVariantList &args) const
{
    // Parse QVariantList from QML to create connection
    // Example: Extract name, device, ipv4 settings

    // return NetworkManager::findConnection(args[1].toString()); // connectionPath
    static const QLatin1String uuidArgumentMarker{"Uuid="};
    for (QVariant arg : args) {
        if (arg.canConvert(QMetaType::QString)) {
            QString uuid = arg.toString();
            if (uuid.startsWith(uuidArgumentMarker)) {
                uuid = uuid.replace(uuidArgumentMarker, QString());
                return NetworkManager::findConnectionByUuid(uuid);
            }
        }
    }
    return nullptr;
}

