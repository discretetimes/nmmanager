#include "connectionproxymodel.h"
#include "networkmodel.h"

ConnectionProxyModel::ConnectionProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterRole(NetworkModel::NameRole);
    // setFilterCaseSensitivity(Qt::CaseInsensitive);
    // setSortCaseSensitivity(Qt::CaseInsensitive);
    // setSortLocaleAware(true);
    sort(0, Qt::DescendingOrder);
}

bool ConnectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    // const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
    // const bool isDuplicate = sourceModel()->data(index, NetworkModel::DuplicateRole).toBool();

    // if (isSlave || isDuplicate) {
    //     return false;
    // }

    const NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType)sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
    // qInfo() << "connectionType:" << type << "connectiontype" << NetworkManager::ConnectionSettings::ConnectionType::Wired;
    if (type != NetworkManager::ConnectionSettings::ConnectionType::Wired) {
        return false;
    }
    // QString connectionType = sourceModel()->data(index, NetworkModel::TypeRole).toString();
    // "Loopback" type 20 in kf6, networkmanager upper version
    // QString connectionName = sourceModel()->data(index, NetworkModel::NameRole).toString();
    // if (connectionName == "lo") {
    //     return false;
    // }
    // else {
    // qInfo() << "connectionType:" << connectionType;
    // return connectionType == "13";
    // }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
