/*
    SPDX-FileCopyrightText: 2013-2018 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "networkmodelitem.h"
#include "networkmodel.h"

#include <unordered_map>

#include <NetworkManagerQt/AdslDevice>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/BondDevice>
#include <NetworkManagerQt/BridgeDevice>
#include <NetworkManagerQt/InfinibandDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/TeamDevice>
#include <NetworkManagerQt/VlanDevice>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

#include <QStringBuilder>

namespace
{
using ConnectionType = NetworkManager::ConnectionSettings::ConnectionType;

static const std::unordered_map<ConnectionType, QString> s_connectionTypeToString{
                                                                                  {ConnectionType::Unknown, "The connection type is Unknown"},
                                                                                  {ConnectionType::Wired, "The connection type is Wired"},
    };
}

NetworkModelItem::NetworkModelItem(QObject *parent)
    : QObject(parent)
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    // , m_deviceState(NetworkManager::Device::UnknownState)
    // , m_detailsValid(false)
    , m_delayModelUpdates(false)
    // , m_duplicate(false)
    // , m_mode(NetworkManager::WirelessSetting::Infrastructure)
    // , m_securityType(NetworkManager::NoneSecurity)
    , m_signal(0)
    // , m_slave(false)
    , m_type(NetworkManager::ConnectionSettings::Unknown)
    // , m_vpnState(NetworkManager::VpnConnection::Unknown)
    // , m_rxBytes(0)
    // , m_txBytes(0)
    // , m_icon(QStringLiteral("network-wired"))
{
}

NetworkModelItem::NetworkModelItem(const NetworkModelItem *item, QObject *parent)
    : QObject(parent)
    , m_connectionPath(item->connectionPath())
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    // , m_detailsValid(false)
    , m_delayModelUpdates(item->delayModelUpdates())
    // , m_duplicate(true)
    , m_name(item->name())
    , m_type(item->type())
    , m_uuid(item->uuid())
    // , m_vpnState(NetworkManager::VpnConnection::Unknown)
    // , m_rxBytes(0)
    // , m_txBytes(0)
    // , m_icon(item->icon())
{
}

NetworkModelItem::~NetworkModelItem() = default;

QString NetworkModelItem::activeConnectionPath() const
{
    return m_activeConnectionPath;
}

void NetworkModelItem::setActiveConnectionPath(const QString &path)
{
    m_activeConnectionPath = path;
}

QString NetworkModelItem::connectionPath() const
{
    return m_connectionPath;
}

void NetworkModelItem::setConnectionPath(const QString &path)
{
    if (m_connectionPath != path) {
        m_connectionPath = path;
        m_changedRoles << NetworkModel::ConnectionPathRole << NetworkModel::UniRole;
    }
}

NetworkManager::ActiveConnection::State NetworkModelItem::connectionState() const
{
    return m_connectionState;
}

QStringList NetworkModelItem::details() const
{
    // if (!m_detailsValid) {
        updateDetails();
    // }
    return m_details;
}

QString NetworkModelItem::devicePath() const
{
    return m_devicePath;
}

NetworkModelItem::ItemType NetworkModelItem::itemType() const
{
    if (!m_devicePath.isEmpty() //
        || m_type == NetworkManager::ConnectionSettings::Bond //
        || m_type == NetworkManager::ConnectionSettings::Bridge //
        || m_type == NetworkManager::ConnectionSettings::Vlan //
        || m_type == NetworkManager::ConnectionSettings::Team //
        || ((NetworkManager::status() == NetworkManager::Connected //
             || NetworkManager::status() == NetworkManager::ConnectedLinkLocal //
             || NetworkManager::status() == NetworkManager::ConnectedSiteOnly)
            && (m_type == NetworkManager::ConnectionSettings::Vpn || m_type == NetworkManager::ConnectionSettings::WireGuard))) {
        if (m_connectionPath.isEmpty() && m_type == NetworkManager::ConnectionSettings::Wireless) {
            return NetworkModelItem::AvailableAccessPoint;
        } else {
            return NetworkModelItem::AvailableConnection;
        }
    }
    return NetworkModelItem::UnavailableConnection;
}

QString NetworkModelItem::name() const
{
    return m_name;
}

void NetworkModelItem::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_changedRoles << NetworkModel::ItemUniqueNameRole << NetworkModel::NameRole;
    }
}

NetworkManager::ConnectionSettings::ConnectionType NetworkModelItem::type() const
{
    return m_type;
}

void NetworkModelItem::setType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    m_changedRoles << NetworkModel::TypeRole << NetworkModel::ItemTypeRole << NetworkModel::UniRole;
}

QString NetworkModelItem::uni() const
{
    // if (m_type == NetworkManager::ConnectionSettings::Wireless && m_uuid.isEmpty()) {
        // return m_ssid + '%' + m_devicePath;
    // } else {
        return m_connectionPath + '%' + m_devicePath;
    // }
}

QString NetworkModelItem::uuid() const
{
    return m_uuid;
}

void NetworkModelItem::setUuid(const QString &uuid)
{
    if (m_uuid != uuid) {
        m_uuid = uuid;
        m_changedRoles << NetworkModel::UuidRole;
    }
}

bool NetworkModelItem::delayModelUpdates() const
{
    return m_delayModelUpdates;
}

void NetworkModelItem::setDelayModelUpdates(bool delay)
{
    // special case, does not need m_changedRoles
    m_delayModelUpdates = delay;
}

void NetworkModelItem::updateDetails() const
{
    // m_detailsValid = true;
    // m_details.clear();

    qInfo() << "Update details" << m_details ;
    // if (itemType() == NetworkModelItem::UnavailableConnection) {
    //     return;
    // }

    // NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    // Get IPv[46]Address and related nameservers + IPv[46] default gateway
    // if (device && device->ipV4Config().isValid() && m_connectionState == NetworkManager::ActiveConnection::Activated) {
        // if (!device->ipV4Config().addresses().isEmpty()) {
        //     QHostAddress addr = device->ipV4Config().addresses().first().ip();
        //     if (!addr.isNull() && addr.isGlobal()) {
        //         m_details << "IPv4 Address" << addr.toString();
        //     }
        // }
        // if (!device->ipV4Config().gateway().isEmpty()) {
        //     QString addr = device->ipV4Config().gateway();
        //     if (!addr.isNull()) {
        //         m_details << "IPv4 Default Gateway" << addr;
        //     }
        // }
        // if (!device->ipV4Config().nameservers().isEmpty()) {
        //     QHostAddress addr1 = device->ipV4Config().nameservers().first();
        //     QHostAddress addr2 = device->ipV4Config().nameservers().last();
        //     if (!addr1.isNull()) {
        //         m_details << "IPv4 Primary Nameserver" << addr1.toString();
        //     }
        //     if (!addr2.isNull() && !addr1.isNull()) {
        //         if (addr2 != addr1) {
        //             m_details << "IPv4 Secondary Nameserver" << addr2.toString();
        //         }
        //     }
        // }


    m_details << "name" << "test-1";
    m_details << "ipv4Method" << "1";
    m_details << "addresses" << "192.168.0.1";

        qInfo() << "updated details" << m_details;
    // }

    // if (device && m_connectionState == NetworkManager::ActiveConnection::Activated) {
    //     m_details << "Device" << device->interfaceName();
    // }
}
