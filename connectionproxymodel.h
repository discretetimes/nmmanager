#ifndef CONNECTIONPROXYMODEL_H
#define CONNECTIONPROXYMODEL_H

#include <QAbstractListModel>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>

#include <QSortFilterProxyModel>
#include <NetworkManagerQt/Connection>

class ConnectionProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ConnectionProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // CONNECTIONPROXYMODEL_H
