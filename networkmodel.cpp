#include "networkmodel.h"
#include "networkmodelitem.h"

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
    // Connect to global connection change signals
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectionAdded, this, &NetworkModel::addConnection);
    // connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionRemoved, this, &NetworkModel::removeConnection);
    initialize();
}

// int NetworkModel::rowCount(const QModelIndex &parent) const
// {
//     // For list models, the row count is the size of the list.
//     if (parent.isValid())
//         return 0;
//
//     return m_connections.count();
// }

// QVariant NetworkModel::data(const QModelIndex &index, int role) const
// {
//     if (!index.isValid())
//         return QVariant();
//
//     const NetworkManager::Connection::Ptr &connection = m_connections.at(index.row());
//
//     switch (role) {
//     case NameRole:
//         return connection->name();
//     case UuidRole:
//         return connection->uuid();
//     case ConnectionPathRole:
//         return connection->path();
//     case TypeRole:
//         return connection->settings()->connectionType();
//     }
//
//     return QVariant();
// }

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_list.count()) {
        NetworkModelItem *item = m_list.itemAt(row);

        switch (role) {
            case ConnectionDetailsRole:
                return item->details();
            case ConnectionPathRole:
                return item->connectionPath();
            case ItemUniqueNameRole:
                // if (m_list.returnItems(NetworkItemsList::Name, item->name()).count() > 1) {
                //     return item->originalName();
                // } else {
                    return item->name();
                // }
            case ItemTypeRole:
                return item->itemType();
            case NameRole:
                return item->name();
            case TypeRole:
                return item->type();
            case UniRole:
                return item->uni();
            case UuidRole:
                return item->uuid();
            case DelayModelUpdatesRole:
                return item->delayModelUpdates();
            default:
                break;
        }
    }

    return {};
}


// bool NetworkModel::setData(const QModelIndex &index, const QVariant &value, int role)
// {
//     if (data(index, role) != value) {
//         // Here you would implement logic to update the connection using NetworkManager
//         // For example:
//         // m_connections[index.row()]->settings()->setId(value.toString());
//         // m_connections[index.row()]->update(m_connections[index.row()]->settings()->toMap());
//         emit dataChanged(index, index, {role});
//         return true;
//     }
//     return false;
// }

bool NetworkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int row = index.row();
    const bool delay = value.toBool();

    if (row >= 0 && row < m_list.count() && role == DelayModelUpdatesRole) {
        NetworkModelItem *item = m_list.itemAt(row);
        if (item->delayModelUpdates() != delay) {
            item->setDelayModelUpdates(delay);
            // TODO
            // dataChanged(index, index, QList<int>{DelayModelUpdatesRole});
            updateDelayModelUpdates();
            return true;
        }
    }
    return false;
}

int NetworkModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_list.count();
}

QHash<int, QByteArray> NetworkModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ConnectionDetailsRole] = "ConnectionDetails";
    // roles[ConnectionIconRole] = "ConnectionIcon";
    roles[ConnectionPathRole] = "ConnectionPath";
    // roles[ConnectionStateRole] = "ConnectionState";
    // roles[DeviceName] = "DeviceName";
    // roles[DevicePathRole] = "DevicePath";
    // roles[DeviceStateRole] = "DeviceState";
    // roles[DuplicateRole] = "Duplicate";
    roles[ItemUniqueNameRole] = "ItemUniqueName";
    roles[ItemTypeRole] = "ItemType";
    // roles[LastUsedRole] = "LastUsed";
    roles[NameRole] = "Name";
    // roles[SectionRole] = "Section";
    // roles[SignalRole] = "Signal";
    // roles[SlaveRole] = "Slave";
    // roles[SsidRole] = "Ssid";
    // roles[SpecificPathRole] = "SpecificPath";
    // roles[SecurityTypeRole] = "SecurityType";
    // roles[SecurityTypeStringRole] = "SecurityTypeString";
    // roles[TimeStampRole] = "TimeStamp";
    roles[TypeRole] = "Type";
    // roles[Qt::AccessibleDescriptionRole] = "AccessibleDescription";
    roles[UniRole] = "Uni";
    roles[UuidRole] = "Uuid";
    // roles[VpnState] = "VpnState";
    // roles[VpnType] = "VpnType";
    // roles[RxBytesRole] = "RxBytes";
    // roles[TxBytesRole] = "TxBytes";
    roles[DelayModelUpdatesRole] = "DelayModelUpdates";

    return roles;
}

Qt::ItemFlags NetworkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // Or whatever flags you need
}

// QHash<int, QByteArray> NetworkModel::roleNames() const
// {
//     QHash<int, QByteArray> roles;
//     roles[NameRole] = "name";
//     roles[UuidRole] = "uuid";
//     roles[ConnectionPathRole] = "path";
//     roles[TypeRole] = "type";
//     return roles;
// }

void NetworkModel::initialize()
{
    beginResetModel();
    qInfo() << "Initialize: ";

    // Initialize existing connections
    for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
        addConnection(connection);
    }

    initializeSignals();
    // m_connections = NetworkManager::listConnections();
    // qInfo() << "Wired con count:" << m_connections.count();
    // for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
    //     // insertConnection(connection);
    //     qInfo() << "Con name:" << connection->name() << "con type:" << connection->settings()->connectionType();
    // }
    // endResetModel();
}

void NetworkModel::initializeSignals()
{
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::activeConnectionAdded, this, &NetworkModel::activeConnectionAdded, Qt::UniqueConnection);
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::activeConnectionRemoved, this, &NetworkModel::activeConnectionRemoved, Qt::UniqueConnection);
    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, this, &NetworkModel::connectionAdded, Qt::UniqueConnection);
    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionRemoved, this, &NetworkModel::connectionRemoved, Qt::UniqueConnection);
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &NetworkModel::deviceAdded, Qt::UniqueConnection);
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceRemoved, this, &NetworkModel::deviceRemoved, Qt::UniqueConnection);
    // connect(NetworkManager::notifier(), &NetworkManager::Notifier::statusChanged, this, &NetworkModel::statusChanged, Qt::UniqueConnection);
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


// void NetworkModel::addConnection(NMConnection *connection)
// {
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
// }


void NetworkModel::insertItem(int index)
{
    emit beginInsertRows(QModelIndex(), index, index);
    m_connections.removeAt(index);
    emit endInsertRows();;
}

void NetworkModel::removeItem(int index)
{
    emit beginRemoveRows(QModelIndex(), index, index);
    m_connections.removeAt(index);
    emit endRemoveRows();
}

// void NetworkModel::updateItem(const QModelIndex &index, const QVariant &value, int role)
// {
//     // const int row = m_list.indexOf(item);
//     // if (row != -1) {
//     //     item->invalidateDetails();
//     //     QModelIndex index = createIndex(row, 0);
//     //     Q_EMIT dataChanged(index, index, item->changedRoles());
//     //     item->clearChangedRoles();
//     // }
//     if (data(index, role) != value) {
//         // Here you would implement logic to update the connection using NetworkManager
//         // For example:
//         // m_connections[index.row()]->settings()->setId(value.toString());
//         // m_connections[index.row()]->update(m_connections[index.row()]->settings()->toMap());
//         emit dataChanged(index, index, {role});
//         return true;
//     }
//     return false;
// }

void NetworkModel::addConnection(const NetworkManager::Connection::Ptr &connection)
{
    // Can't add a connection without name or uuid
    if (connection->name().isEmpty() || connection->uuid().isEmpty()) {
        return;
    }

    initializeSignals(connection);

    NetworkManager::ConnectionSettings::Ptr settings = connection->settings();


    // Check whether the connection is already in the model to avoid duplicates, but this shouldn't happen
    if (m_list.contains(NetworkItemsList::Connection, connection->path())) {
        return;
    }

    auto item = new NetworkModelItem();
    item->setConnectionPath(connection->path());
    item->setName(settings->id());
    item->setType(settings->connectionType());
    item->setUuid(settings->uuid());

    insertItem(item);
    qInfo() << "New connection" << item->name() << "added";
}

void NetworkModel::initializeSignals(const NetworkManager::Connection::Ptr &connection)
{
    connect(connection.data(), &NetworkManager::Connection::updated, this, &NetworkModel::connectionUpdated, Qt::UniqueConnection);
}

void NetworkModel::connectionUpdated()
{
    auto connectionPtr = qobject_cast<NetworkManager::Connection *>(sender());
    if (!connectionPtr) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr settings = connectionPtr->settings();
    for (NetworkModelItem *item : m_list.returnItems(NetworkItemsList::Connection, connectionPtr->path())) {
        item->setConnectionPath(connectionPtr->path());
        item->setName(settings->id());
        item->setType(settings->connectionType());
        item->setUuid(settings->uuid());

        updateItem(item);
        qInfo() << "Item " << item->name() << ": connection updated";
    }
}

void NetworkModel::connectionAdded(const QString &connection)
{
    NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(connection);
    if (newConnection) {
        addConnection(newConnection);
    }
}

void NetworkModel::connectionRemoved(const QString &connection)
{
    bool remove = false;
    for (NetworkModelItem *item : m_list.returnItems(NetworkItemsList::Connection, connection)) {
            remove = true;

        if (remove) {
            removeItem(item);
            qInfo() << "Item" << item->name() << "removed completely";
        }
        remove = false;
    }
}

void NetworkModel::addConnectionFromMap(const NMVariantMapMap &map)
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
    qInfo() << "deleting connection: " << connection;
    // NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connection);
    qInfo() << "deleting connection: " << con->name();
    con->remove();
    // for (int i = 0; i < m_connections.size(); ++i) {
    //     if (m_connections[i]->uuid() == connection) {
    //         m_connections.removeAt(i);
    //         // m_networkModel->removeConnection(path);
    //         removeItem(i);
    //         break;
    //     }
    // }

    // removeConnectionInternal(connection);
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
/*
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
}*/

// QVariantMap NetworkModel::getConnectionDetails(const QString &uuid)
// {
//     NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(uuid);
//     ConnectionSettings::Ptr connection = con->settings();
//     // QVariantMap details;
//     // details["name"] = connection->name();
//     NetworkManager::WiredSetting::Ptr wiredSetting = connection->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
//     // if (wiredSetting) {
//     //     details["device"] = wiredSetting->device();
//     // }
//
//     NetworkManager::Ipv4Setting::Ptr ipv4Setting = connection->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
//     if (ipv4Setting) {
//         QVariantMap ipv4Details;
//         ipv4Details["method"] = ipv4Setting->method();
//         // You would need to properly handle addresses, as they are a list of Address objects
//         // This is a simplified example
//         // if (!ipv4Setting->addresses().isEmpty()) {
//         //     ipv4Details["address"] = ipv4Setting->addresses().first().address();
//         // }
//         // details["ipv4"] = ipv4Details;
//     }
//     // NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
//     NMVariantMapMap allSettings = connection->toMap();
//     qInfo() << "Connection details: " <<  allSettings;
//     return allSettings;
// }


QVariantMap NetworkModel::getConnectionDetails(const QString &uuid)
{
    QVariantMap details;
    // for (const auto &connection : m_connections) {
        // if (connection->uuid() == uuid) {
            NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);
            details["name"] = connection->name();
            details["uuid"] = connection->uuid();
            details["path"] = connection->path();
            details["type"] = connection->settings()->connectionType();

            // Get IPv4 settings
            if (auto ipv4Setting = connection->settings()->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>()) {
                details["ipv4Method"] = [ipv4Setting]() -> QString {
                    switch (ipv4Setting->method()) {
                        case Ipv4Setting::Automatic: return "auto";
                        case Ipv4Setting::Manual: return "manual";
                        case Ipv4Setting::LinkLocal: return "link-local";
                        case Ipv4Setting::Shared: return "shared";
                        case Ipv4Setting::Disabled: return "disabled";
                        default: return "unknown";
                    }
                }();

                QStringList addresses;
                for (const auto &addr : ipv4Setting->addresses()) {
                    addresses << addr.ip().toString() + "/" + QString::number(addr.prefixLength());
                }
                details["ipv4Addresses"] = addresses;
            }

            // Get device interface
            // details["deviceInterface"] = connection->interfaceName();
            // break;
        // }
    // }""
    qInfo() << "Get details: " << details;
    return details;
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

void NetworkModel::insertItem(NetworkModelItem *item)
{
    if (m_delayModelUpdates) {
        m_updateQueue.enqueue(QPair<NetworkModel::ModelChangeType, NetworkModelItem *>(NetworkModel::ItemAdded, item));
        return;
    }

    const int index = m_list.count();
    beginInsertRows(QModelIndex(), index, index);
    m_list.insertItem(item);
    endInsertRows();
    updateDelayModelUpdates();
}

void NetworkModel::removeItem(NetworkModelItem *item)
{
    if (m_delayModelUpdates) {
        m_updateQueue.enqueue(QPair<NetworkModel::ModelChangeType, NetworkModelItem *>(NetworkModel::ItemRemoved, item));
        return;
    }

    const int row = m_list.indexOf(item);
    if (row != -1) {
        beginRemoveRows(QModelIndex(), row, row);
        m_list.removeItem(item);
        item->deleteLater();
        endRemoveRows();
        updateDelayModelUpdates();
    }
}

void NetworkModel::updateItem(NetworkModelItem *item)
{
    if (m_delayModelUpdates) {
        m_updateQueue.enqueue(QPair<NetworkModel::ModelChangeType, NetworkModelItem *>(NetworkModel::ItemPropertyChanged, item));
        return;
    }

    const int row = m_list.indexOf(item);
    if (row != -1) {
        // item->invalidateDetails();
        QModelIndex index = createIndex(row, 0);
        // Q_EMIT dataChanged(index, index, item->changedRoles());
        item->clearChangedRoles();
        updateDelayModelUpdates();
    }
}

bool NetworkModel::delayModelUpdates() const
{
    return m_delayModelUpdates;
}

void NetworkModel::flushUpdateQueue()
{
    while (!m_updateQueue.isEmpty()) {
        QPair<NetworkModel::ModelChangeType, NetworkModelItem *> update = m_updateQueue.dequeue();
        if (update.first == ItemAdded) {
            insertItem(update.second);
        } else if (update.first == ItemRemoved) {
            removeItem(update.second);
        } else if (update.first == ItemPropertyChanged) {
            updateItem(update.second);
        }
    }
}

void NetworkModel::updateDelayModelUpdates()
{
    const QList<NetworkModelItem *> items = m_list.items();
    const bool delay = std::any_of(items.begin(), items.end(), [](NetworkModelItem *item) -> bool {
        return item->delayModelUpdates();
    });
    if (m_delayModelUpdates != delay) {
        m_delayModelUpdates = delay;
        Q_EMIT delayModelUpdatesChanged();

        if (!m_delayModelUpdates) {
            flushUpdateQueue();
        }
    }
}

