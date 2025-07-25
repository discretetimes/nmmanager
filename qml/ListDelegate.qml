import QtQuick 2.15
import QtQuick.Layouts 1.2

Item {
    id: root
    width: ListView.view.width
    height: 48
    property alias text: label.text
    property alias color: label.color

    signal clicked()
    signal remove()

    Rectangle {
        anchors.fill: parent
        color: '#ffffff'
        opacity: 0.2
        border.color: Qt.darker(color)
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 2
        anchors.rightMargin: 2
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: label
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 2
                color: '#f7f7f7'
                font.pixelSize: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.push(Details.qml)
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: icon.width
            Image {
                id: icon
                anchors.centerIn: parent
                source: 'remove.png'
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.remove()
            }
        }
    }
}
