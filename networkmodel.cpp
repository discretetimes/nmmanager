#include "networkmodel.h"

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

void add_connection_cb(GObject *client, GAsyncResult *result, gpointer user_data)
{
    AddConnectionData *data = static_cast<AddConnectionData *>(user_data);

    GError *error = nullptr;
    NMRemoteConnection *connection = nm_client_add_connection2_finish(NM_CLIENT(client), result, NULL, &error);

    if (error) {
        // KNotification *notification = new KNotification(QStringLiteral("FailedToAddConnection"), KNotification::CloseOnTimeout, data->handler);
        // notification->setTitle(i18n("Failed to add connection %1", data->id));
        // notification->setComponentName(QStringLiteral("networkmanagement"));
        // notification->setText(QString::fromUtf8(error->message));
        // notification->setIconName(QStringLiteral("dialog-warning"));
        // notification->sendEvent();

        g_error_free(error);
    } else {
        // KNotification *notification = new KNotification(QStringLiteral("ConnectionAdded"), KNotification::CloseOnTimeout, data->handler);
        // notification->setText(i18n("Connection %1 has been added", data->id));
        // notification->setComponentName(QStringLiteral("networkmanagement"));
        // notification->setTitle(data->id);
        // notification->setIconName(QStringLiteral("dialog-information"));
        // notification->sendEvent();

        g_object_unref(connection);
    }

    delete data;
}


void NetworkModel::addConnection(NMConnection *connection)
{
    NMClient *client = nm_client_new(nullptr, nullptr);

    AddConnectionData *userData = new AddConnectionData{QString::fromUtf8(nm_connection_get_id(connection)), this};

    nm_client_add_connection2(client,
                              nm_connection_to_dbus(connection, NM_CONNECTION_SERIALIZE_ALL),
                              NM_SETTINGS_ADD_CONNECTION2_FLAG_TO_DISK,
                              nullptr,
                              true,
                              nullptr,
                              add_connection_cb,
                              userData);
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

void NetworkModel::updateConnection(NetworkManager::Connection::Ptr connection, const NMVariantMapMap &map)
{
        // connection->update(map);
    QDBusReply<void> reply = connection->update(map);
}

void NetworkModel::setConnection(const NetworkManager::ConnectionSettings::Ptr& connection)
{
    QVariantMap details;
    details["name"] = connection->name();
}


