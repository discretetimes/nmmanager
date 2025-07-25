import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    title: qsTr("Network Connections")

    // Handle connectionsChanged signal from networkModel
    // Connections {
    //     target: networkModel
    //     function onConnectionsChanged() {
    //         // Force ListView to update by resetting the model
    //         var m = networkListView.model;
    //         networkListView.model = null;
    //         networkListView.model = m;
    //     }
    // }
    // Properties to track selected connection
    property string currentConnectionUuid: ""
    property var currentConnectionDetails: ({})
    // property string currentConnectionName: ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16 // Larger margins for touch-friendly spacing

        ListView {
            id: networkListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: connectionProxyModel
            currentIndex: -1
            spacing: 8 // Space between list items for touch
            clip: true // Prevent content from spilling outside

            delegate: ItemDelegate {
                width: parent.width
                height: 64 // Larger height for touch targets
                text: model.Name
                // highlighted: ListView.isCurrentItem
                // highlighted: networkListView.currentConnectionUuid === model.uuid
                highlighted: currentConnectionUuid === model.uuid

                // Customize appearance for touch
                // contentItem: Text {
                //     text: model.name
                //     font.pixelSize: 18 // Larger text for readability
                //     color: highlighted ? "white" : "black"
                //     verticalAlignment: Text.AlignVCenter
                //     padding: 12 // Padding for touch comfort
                // }

                background: Rectangle {
                    color: highlighted ? "#0078d7" : "white" // Clear highlight color
                    radius: 8 // Rounded corners for modern look
                    border.color: "#e0e0e0"
                    border.width: highlighted ? 2 : 1
                }

                onClicked: {
                    // networkListView.currentIndex = index
                    // networkListView.currentConnectionUuid = model.uuid
                    currentConnectionUuid = model.Uuid
                    console.info("Details: ", currentConnectionUuid)
                    currentConnectionDetails = model.ConnectionDetails
                    console.info("Details: ", currentConnectionDetails)
                }
            }

            // Smooth scrolling for touch
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar { active: true }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 12 // Increased spacing between buttons

            Button {
                text: qsTr("Add")
                Layout.preferredWidth: 120 // Wider buttons for touch
                Layout.preferredHeight: 48
                font.pixelSize: 16

                // Touch feedback
                background: Rectangle {
                    color: pressed ? "#005bb5" : "#0078d7"
                    radius: 8
                }

                onClicked: {
                    stackView.push("AddConnectionPage.qml")
                }
            }

            Button {
                text: qsTr("Modify")
                // enabled: networkListView.currentIndex != -1
                enabled: currentConnectionUuid !== ""
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                font.pixelSize: 16

                background: Rectangle {
                    color: pressed ? "#005bb5" : enabled ? "#0078d7" : "#cccccc"
                    radius: 8
                }

                onClicked: {
                    // var selectedUuid = networkListView.model.get(networkListView.currentIndex).uuid
                    console.info("Modifying connection:", currentConnectionUuid)
                    // console.info("Clicked on connection with UUID:", selectedUuid)
                    nmManager.onSelectedConnectionChanged(currentConnectionUuid)
                    // nmManager.onSelectedConnectionChanged(selectedUuid)
                    stackView.push("EditingEthernetPage.qml", { "connectionUuid": currentConnectionUuid })
                }
            }

            Button {
                text: qsTr("Connect")
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                font.pixelSize: 16

                background: Rectangle {
                    color: pressed ? "#005bb5" : "#0078d7"
                    radius: 8
                }

                // deactive, if not actived
                // var DevicePath = "enp7s18u4u2c2"
                onClicked: {
                    // nmManager.activateConnection(currentConnectionUuid, DevicePath)
                    nmManager.activateConnection(currentConnectionUuid, "enp7s18u5u3c2")
                }

            }

            Button {
                text: qsTr("Delete")
                // enabled: networkListView.currentIndex != -1
                enabled: currentConnectionUuid !== ""
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                font.pixelSize: 16

                background: Rectangle {
                    color: pressed ? "#005bb5" : "#0078d7"
                    radius: 8
                }

                onClicked: {
                    // var uuid = networkListView.model.get(networkListView.currentIndex).uuid
                    console.info("connection:", currentConnectionUuid)
                    nmManager.removeConnection(currentConnectionUuid)
                }
            }
        }
    }
}
