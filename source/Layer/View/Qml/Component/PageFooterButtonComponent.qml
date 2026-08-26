import QtQuick
import QtQuick.Controls.Basic

RoundButton {
    id: root

    implicitWidth: 100
    implicitHeight: 26
    radius: height / 2
    palette.buttonText: "white"

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
