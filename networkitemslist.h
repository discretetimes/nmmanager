#ifndef NETWORKITEMSLIST_H
#define NETWORKITEMSLIST_H

#include <QAbstractListModel>

#include <NetworkManagerQt/ConnectionSettings>

class NetworkModelItem;

class NetworkItemsList : public QObject
{
    Q_OBJECT
public:
    enum FilterType {
        ActiveConnection,
        Connection,
        Device,
        Name,
        Ssid,
        Uuid,
        Type,
    };

    explicit NetworkItemsList(QObject *parent = nullptr);
    ~NetworkItemsList() override;

    bool contains(const FilterType type, const QString &parameter) const;
    int count() const;
    int indexOf(NetworkModelItem *item) const;
    NetworkModelItem *itemAt(int index) const;
    QList<NetworkModelItem *> items() const;
    QList<NetworkModelItem *> returnItems(const FilterType type, const QString &parameter, const QString &additionalParameter = QString()) const;
    QList<NetworkModelItem *> returnItems(const FilterType type, NetworkManager::ConnectionSettings::ConnectionType typeParameter) const;

    void insertItem(NetworkModelItem *item);
    void removeItem(NetworkModelItem *item);

private:
    QList<NetworkModelItem *> m_items;
};

#endif // NETWORKITEMSLIST_H
