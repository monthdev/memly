import QtQuick
import QtQuick.Controls.Basic

Page {
    id: root

    required property color pageColor

    background: Rectangle {
        color: root.pageColor
    }

    Label {
        anchors.centerIn: parent
        text: "Edit Card"
        color: "white"
        font.pixelSize: 28
        font.bold: true
    }
}
