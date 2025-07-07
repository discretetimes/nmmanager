#include "networkmodel.h"

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

void NetworkModel::addConnection(NMConnection *connection)
{
        // beginInsertRows({}, rowCount(), rowCount());
        // m_connections.append(connection);
        // endInsertRows();
    }

void NetworkModel::addConnection(const NMVariantMapMap &map)
{
        // NetworkManager::addConnection(map);
        // beginResetModel();
        // m_connections = NetworkManager::listConnections();
        // endResetModel();
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
}

void NetworkModel::updateConnection(NetworkManager::Connection::Ptr connection, const NMVariantMapMap &map) {
        // connection->update(map);
}



