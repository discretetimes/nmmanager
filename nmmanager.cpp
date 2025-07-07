#include "networkmodel.h"
#include "nmmanager.h"

using namespace NetworkManager;

NmManager::NmManager(QObject* parent)
    : QObject (parent)
{
}

void NmManager::onRequestCreateConnection(int connectionType)
{
        // NetworkManager::ConnectionSettings::Ptr settings(new NetworkManager::ConnectionSettings);
        // settings->setType(static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connectionType));
        // settings->setId("con" + QString::number(m_networkModel->rowCount() + 1));
        // settings->setAutoconnect(true); // Default
        // addConnection(settings);
    }

void NmManager::onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath) {
        auto connection = NetworkManager::findConnection(connectionPath);
        if (connection) {
            // loadConnectionSettings(connection->settings());
        }
}

void NmManager::addConnection(const NetworkManager::ConnectionSettings::Ptr &settings) {
        // NMVariantMapMap map = settings->toMap();
        // m_networkModel->addConnection(map);
}

void NmManager::loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &settings) {
        // Emit signal to QML with settings data
        // Q_EMIT connectionLoaded(settings->toMap());
}

NetworkManager::Connection::Ptr NmManager::connectionFromArgs(const QVariantList &args) const
{
        // Parse QVariantList from QML to create connection
        // Example: Extract name, device, ipv4 settings
        return NetworkManager::findConnection(args[1].toString()); // connectionPath
}
