import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 960
    height: 600
    minimumWidth: 400
    minimumHeight: 90
    visible: true
    title: "Memly"
    color: "#121212"

    Text {
        anchors.centerIn: parent
        text: "Hello"
        color: "white"
        font.pixelSize: 48
    }
}
