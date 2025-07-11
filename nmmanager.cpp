#include "nmmanager.h"

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

NmManager::NmManager(QObject* parent)
    : QObject (parent)
{
}

void NmManager::onRequestCreateConnection(const QVariantMap &settings)
{
    // auto type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(NetworkManager::ConnectionSettings::Wired);
    // if (type == NetworkManager::ConnectionSettings::Wired){
        // NetworkManager::ConnectionSettings::Ptr connectionSettings;
        ConnectionSettings::Ptr newSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wired));

        // connection settings
        // default type wired
        // NetworkManager::WiredSetting::Ptr wiredSetting =
        // connectionSettings->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
        // create new uuid
        qInfo() << "Add settings: " << settings;
        newSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        // create connection name
        if (settings.contains("name")) {
            newSettings->setId(settings["name"].toString());
        }
        // ipv4 settings
        Ipv4Setting::Ptr m_ipv4setting = newSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
        m_ipv4setting->setInitialized(true);
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
        NMVariantMapMap newSettingsMap = newSettings->toMap();
        qInfo() << "Add con settings: " << newSettingsMap;
        QDBusReply<QDBusObjectPath> reply = NetworkManager::addConnection(newSettingsMap);
        qInfo() << "Add settings results: " << reply.error().message();
        // NetworkModel->addConnection(newSettingsMap);
}

// void NmManager::onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath) {
//     NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
//         if (connection) {
//             // loadConnectionSettings(connection->settings());
//         }
// }

// void NmManager::onSelectedConnectionChanged(const QString &connectionPath)
// {
//     m_currentConnectionPath = connectionPath;
//     NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
//     if (connection) {
//         NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
//         loadConnectionSettings(connectionSettings);
//     }
// }

void NmManager::onSelectedConnectionChanged(const QString &connectionUuid)
{
    QVariantList args;
    args << "Uuid=" + connectionUuid;
    NetworkManager::Connection::Ptr connection = connectionFromArgs(args);
    // m_currentConnectionPath = connectionPath;
    // NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        loadConnectionSettings(connectionSettings);
    }
}

// void NmManager::addConnection(const NetworkManager::ConnectionSettings::Ptr &settings) {
//         // NMVariantMapMap map = settings->toMap();
//         // m_networkModel->addConnection(map);
// }

void NmManager::loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &settings) {
    // Emit signal to QML with settings data
    // Q_EMIT connectionLoaded(settings->toMap());
    m_networkModel->setConnection(settings);
}

NetworkManager::Connection::Ptr NmManager::connectionFromArgs (const QVariantList &args) const
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

void NmManager::removeConnection(const QString &uuid)
{
    // qInfo() << "deleting connection: " << uuid;
    // NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    // NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(uuid);
    // qInfo() << "deleting connection: " << con->name();
    // con->remove();
     m_networkModel->removeConnection(uuid);
}


// QVariantMap NmManager::get(int row)
// {
//     if (row < 0 || row >= m_connections.count())
//         return QVariantMap();
//
//     QVariantMap res;
//     const auto &connection = m_connections.at(row);
//     res["name"] = connection->name();
//     res["uuid"] = connection->uuid();
//     return res;
// }
