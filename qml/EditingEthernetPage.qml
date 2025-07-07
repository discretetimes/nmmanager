import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: editingEthernetPage
    title: "Edit Ethernet Connection"

    property alias connectionName: nameField.text
    property alias device: deviceComboBox.currentText
    property alias ipv4Method: ipv4MethodComboBox.currentText
    property alias ipv4Address: ipAddressField.text

    Connections {
        target: nmManager
        function onConnectionLoaded(settings) {
            nameField.text = settings.name
            // Assuming 'device' and 'ipv4' settings will be added to the emitted map
            if (settings.device) {
                deviceComboBox.currentIndex = deviceComboBox.find(settings.device)
            }
            if (settings.ipv4) {
                ipv4MethodComboBox.currentIndex = ipv4MethodComboBox.find(settings.ipv4.method)
                ipAddressField.text = settings.ipv4.address
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Label {
            text: "Name:"
            font.bold: true
        }

        TextField {
            id: nameField
            placeholderText: "Enter connection name"
            Layout.fillWidth: true
        }

        Label {
            text: "Ethernet Device:"
            font.bold: true
        }

        ComboBox {
            id: deviceComboBox
            model: ["eth0", "eth1", "enp2s0"] // Example devices
            Layout.fillWidth: true
        }

        Label {
            text: "IPv4 Settings:"
            font.bold: true
        }

        ComboBox {
            id: ipv4MethodComboBox
            model: ["Automatic (DHCP)", "Manual"]
            Layout.fillWidth: true
        }

        TextField {
            id: ipAddressField
            placeholderText: "Enter IP address (e.g., 192.168.1.100)"
            Layout.fillWidth: true
            visible: ipv4MethodComboBox.currentText === "Manual"
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 10

            Button {
                text: "Save"
                onClicked: {
                    // Implement save logic here
                    // You can call a Q_INVOKABLE function on nmManager
                }
            }

            Button {
                text: "Cancel"
                onClicked: {
                    // Implement cancel logic here
                    // For example, close the page
                }
            }
        }
    }
}
