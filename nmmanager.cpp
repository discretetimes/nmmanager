#include "nmmanager.h"

#include "networkmodel.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredSetting>

using namespace NetworkManager;

NmManager::NmManager(QObject* parent)
    : QObject (parent)
{
}

void NmManager::onRequestCreateConnection(int connectionType)
{
    auto type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connectionType);
    if (type == NetworkManager::ConnectionSettings::Wired){
        NetworkManager::ConnectionSettings::Ptr connectionSettings;
        NetworkManager::WiredSetting::Ptr wiredSetting =
        connectionSettings->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        addConnection(connectionSettings);
    }
}

// void NmManager::onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath) {
//     NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
//         if (connection) {
//             // loadConnectionSettings(connection->settings());
//         }
// }

void NmManager::onSelectedConnectionChanged(const QString &connectionPath)
{
    m_currentConnectionPath = connectionPath;
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        loadConnectionSettings(connectionSettings);
    }
}

void NmManager::addConnection(const NetworkManager::ConnectionSettings::Ptr &settings) {
        // NMVariantMapMap map = settings->toMap();
        // m_networkModel->addConnection(map);
}

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
