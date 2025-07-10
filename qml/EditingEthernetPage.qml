import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    property string connectionUuid
    property var connectionDetails: ({})

    title: "Edit Addresses for " + (connectionDetails.name || "Connection")

    ListModel {
        id: addressModel
    }

    Component.onCompleted: {
        // Fetch connection details
        connectionDetails = networkModel.getConnectionDetails(connectionUuid) || {}
        if (!connectionDetails.name) {
            console.warn("No connection details found for UUID:", connectionUuid)
        }

        // Set IPv4 method
        // methodComboBox.currentIndex = (connectionDetails.ipv4Method === 0) ? 0 : 1
        if (connectionDetails.ipv4Method === 0) {
            methodComboBox.currentIndex = 0
        } else {
            methodComboBox.currentIndex = 1
        }

        // Populate address model
        // if (Array.isArray(connectionDetails.addresses)) {
        //     for (var i = 0; i < connectionDetails.addresses.length; ++i) {
        //         addressModel.append({ "address": connectionDetails.addresses[i] })
        //     }
        // } else {
        //     console.warn("Invalid or missing addresses in connectionDetails")
        // }
        for (var i = 0; i < connectionDetails.addresses.length; ++i) {
            addressModel.append({ "address": connectionDetails.addresses[i] })
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 16 // Larger margins for touch
        spacing: 12 // Increased spacing between elements

        Label {
            text: qsTr("IPv4 Method:")
            font.pixelSize: 18 // Larger text for readability
        }

        ComboBox {
            id: methodComboBox
            Layout.fillWidth: true
            Layout.preferredHeight: 56 // Larger touch target
            font.pixelSize: 18
            model: ["Automatic (DHCP)", "Manual"]

            background: Rectangle {
                color: "white"
                border.color: methodComboBox.activeFocus ? "#0078d7" : "#e0e0e0"
                border.width: methodComboBox.activeFocus ? 2 : 1
                radius: 8
            }
            contentItem: Text {
                text: methodComboBox.displayText
                font: methodComboBox.font
                color: methodComboBox.enabled ? "black" : "#666666"
                verticalAlignment: Text.AlignVCenter
                leftPadding: 12
                rightPadding: methodComboBox.indicator.width + methodComboBox.spacing
            }
        }

        ListView {
            id: addressListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: addressModel
            currentIndex: -1
            enabled: methodComboBox.currentIndex === 1
            spacing: 8 // Space between items for touch

            // Smooth scrolling for touch
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar { active: true }

            delegate: ItemDelegate {
                width: parent.width
                height: 64 // Larger touch target
                highlighted: ListView.isCurrentItem

                TextField {
                    id: addressField
                    anchors.fill: parent
                    anchors.margins: 8 // Padding for touch comfort
                    text: model.address
                    placeholderText: parent.enabled ? "e.g., 192.168.1.100/24" : "Not applicable"
                    font.pixelSize: 16 // Larger text
                    readOnly: !parent.enabled

                    background: Rectangle {
                        color: "white"
                        border.color: addressField.activeFocus ? "#0078d7" : "#e0e0e0"
                        border.width: addressField.activeFocus ? 2 : 1
                        radius: 8
                    }

                    onEditingFinished: {
                        model.address = text
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    preventStealing: true

                    onClicked: {
                        addressListView.currentIndex = index
                        addressField.forceActiveFocus()
                    }
                }
            }
        }

        RowLayout {
            enabled: methodComboBox.currentIndex === 1
            Layout.alignment: Qt.AlignHCenter
            spacing: 12 // Increased spacing between buttons

            Button {
                text: qsTr("Add Address")
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                font.pixelSize: 16

                background: Rectangle {
                    color: pressed ? "#005bb5" : "#0078d7"
                    radius: 8
                }

                onClicked: {
                    addressModel.append({ "address": "" })
                    // addressListView.currentIndex = addressModel.count - 1
                    // addressListView.currentItem.children[0].forceActiveFocus() // Focus the new TextField
                }
            }

            Button {
                text: qsTr("Remove Selected")
                enabled: addressListView.currentIndex !== -1
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                font.pixelSize: 16

                background: Rectangle {
                    color: pressed ? "#b00020" : enabled ? "#d32f2f" : "#cccccc"
                    radius: 8
                }

                onClicked: {
                    if (addressListView.currentIndex !== -1) {
                        addressModel.remove(addressListView.currentIndex)
                        // addressListView.currentIndex = -1 // Clear selection
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        Layout.preferredHeight: 48
        spacing: 12

        background: Rectangle {
            color: "#f5f5f5"
            radius: 8
        }

        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            text: qsTr("Save")
            font.pixelSize: 16
            Layout.preferredWidth: 120
            background: Rectangle {
                color: pressed ? "#005bb5" : "#0078d7"
                radius: 8
            }
        }

        Button {
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            text: qsTr("Cancel")
            font.pixelSize: 16
            Layout.preferredWidth: 120
            background: Rectangle {
                color: pressed ? "#666666" : "#888888"
                radius: 8
            }
        }

        onAccepted: {
            var newAddresses = []
            if (methodComboBox.currentIndex === 1) {
                for (var i = 0; i < addressModel.count; i++) {
                    var item = addressModel.get(i)
                    if (item.address && item.address.trim() !== "") {
                        // Basic validation: ensure format is IP/prefix
                        var parts = item.address.split('/')
                        if (parts.length === 2 && parts[0].match(/^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/) && parts[1].match(/^\d{1,2}$/)) {
                            newAddresses.push(item.address)
                        } else {
                            console.warn("Invalid address format skipped:", item.address)
                        }
                    }
                }
                if (newAddresses.length === 0) {
                    console.warn("No valid addresses provided for manual configuration")
                    return // Prevent saving invalid settings
                }
            }
            // var newMethod;
            // if (methodComboBox.currentIndex === 0) {
            //     newMethod = 0;
            // } else {
            //     newMethod = 2;
            // }
            var newSettings = {
                "ipv4Method": methodComboBox.currentIndex === 0 ? 0 : 2,
                "addresses": newAddresses
            }
            networkModel.updateConnection(newSettings)
            stackView.pop()
        }

        onRejected: {
            stackView.pop()
        }
    }
}
