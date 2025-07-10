===================================
networkmanager connection editor
===================================

frontend
===========
参考 nm-connection-editor

Connection between qml & cpp
==============================

Page 1
=========
discription: first page

connection list

listview
connectionView.currentIndex

EtherNet(标题-连接类型)
  sec(连接1)
  auto(连接2)

public:
// create connection
// type == NetworkManager::ConnectionSettings::Wired
//
    Q_INVOKABLE void onRequestCreateConnection(int connectionType);

// click to view details
// NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    Q_INVOKABLE void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);

// select connections
// m_currentConnectionPath = connectionPath;
// NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
// NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
// loadConnectionSettings(connectionSettings);
    Q_INVOKABLE void onSelectedConnectionChanged(const QString &connectionPath);

public Q_SLOTS:
    void defaults() override;
    // m_tabWidget->setConnection(connectionSettings)
    void load() override;
    // updateConnection(connection, m_tabWidget->setting());
    void save() override;

private Q_SLOTS:
// when new connection added, refresh
    void onConnectionAdded(const QString &connection);

private:
    QString m_currentConnectionPath;

add(button) (always availiable)
delete(button) (only a nmcon selected)
modify(button) (only a nmcon selected)

Page 2
========
add page (a default modify page)

name:
ipv4 auto:

descrption: default add ethernet type connection
default: autoconnect

Name: con1 (auto gen name, can_modify)
TODO
Ethernet: devicelist:available devices
IPv4 Settings:
  Method:Auto, Mannul
  Addtion static addresses: table(iptable), add(button), delete(button)

Save Cancel

Page 3
========
modify and show details

nmcli connection add con-name test3 type 802-3-ethernet ipv4.method manual ipv4.addresses 192.168.1.100/24

title: connection name
wired: ? restrict to device
ipv4: method, address, netmask

Save Cancel

Debug
==========
QT_LOGGING_RULES="*.info=true" ./build/Desktop-Debug/nmconeditor

Internal
============
ref plasma-nm

nmmanager
------------
interact with qml, use networkmodel as backend, ref kcm.cpp

// create a wired connection
    Q_INVOKABLE void onRequestCreateConnection(int connectionType);

// change connection
    Q_INVOKABLE void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);

// internal add connection
    void addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);

// internal load connection
    void loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);

// internal connection uuid to connection
    NetworkManager::Connection::Ptr connectionFromArgs(const QVariantList &args) const;

//  NetworkModel ItemRole
// editorProxyModel
// onSelectedConnectionChanged
// void setConnectionPath(const QString &path);
// m_changedRoles << NetworkModel::ConnectionPathRole << NetworkModel::UniRole;

networkmodel
-----------------
backend, ref handler.h, models
    void addConnection(NMConnection *connection);
    //
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

plasma-nm
===========
applet
--------
ref networkmanager applet

kcm
------
in the kde system settings
kcm_connections/qml
main.qml
    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.EditorProxyModel {
        id: editorProxyModel
        sourceModel: connectionModel
    }

connectionitem
list all connections

connection details:
libs/editor/settings/ui/wiredconnectionwidget.ui

kcm_connections/kcm.cpp

libs
------
provide handler and models
and connection details

api
=======

settings
---------------
onnection details:
QMap(

("802-3-ethernet", QMap(("auto-negotiate", QVariant(bool, true))("wake-on-lan", QVariant(uint, 1))))
("connection", QMap(("autoconnect", QVariant(bool, false))("autoconnect-slaves", QVariant(int, -1))("id", QVariant(QString, "test-4"))("lldp", QVariant(int, -1))("metered", QVariant(int, 0))("type", QVariant(QString, "802-3-ethernet"))("uuid", QVariant(QString, "b9fa0607-aaf0-47b1-81f2-cbb51a055605"))))
("ipv4", QMap(("address-data", QVariant(NMVariantMapList, ))("addresses", QVariant(UIntListList, ))("method", QVariant(QString, "manual"))))
("ipv6", QMap(("addr-gen-mode", QVariant(int, 1))("method", QVariant(QString, "auto"))))

)

minimal required

Connection Settings (connection):
id
uuid
type
autoconnect

IPv4 Settings (ipv4):
method: "manual"
address-data: []

ConnectionEditorBase::initialize()
// IPv4 widget
auto ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Setting::Ipv4), this);
// If the connection is not empty (not new) we want to load its secrets
    if (!emptyConnection) {

void KCMNetworkmanagement::save()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        m_handler->updateConnection(connection, m_tabWidget->setting());
    }
    kcmChanged(false);
    KCModule::save();
}

NMVariantMapMap ConnectionEditorBase::setting() const{
// Set properties which are not returned from setting widgets
NMVariantMapMap settings = m_connectionWidget->setting();
NetworkManager::ConnectionSettings::Ptr connectionSettings =
        NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(m_connection->connectionType()));
connectionSettings->fromMap(settings);
connectionSettings->setId(connectionName());
return connectionSettings->toMap();
}


NMVariantMapMap ConnectionWidget::setting() const{
NetworkManager::ConnectionSettings settings;

return settings.toMap();
}

// monitor changes
QVariantMap IPv4Widget::setting() const
{
NetworkManager::Ipv4Setting ipv4Setting;
ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Automatic);

    if (m_ui->tableViewAddresses->isEnabled()) {
        QList<NetworkManager::IpAddress> list;
        for (int i = 0, rowCount = d->model.rowCount(); i < rowCount; i++) {
            NetworkManager::IpAddress address;
            address.setIp(QHostAddress(d->model.item(i, 0)->text()));
            address.setNetmask(QHostAddress(d->model.item(i, 1)->text()));
            address.setGateway(QHostAddress(d->model.item(i, 2)->text()));
            list << address;
        }
        if (!list.isEmpty()) {
            ipv4Setting.setAddresses(list);
        }
    }

return ipv4Setting.toMap();
}
