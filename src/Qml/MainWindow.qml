import QtQuick
import QtQuick.Controls
import QtCore

ApplicationWindow {
    id: window
    visible: true
    title: "Memly"
    color: "#121212"

    width: 960
    height: 600
    minimumWidth: 400
    minimumHeight: 90

    Settings {
        category: "MainWindow"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Text {
        anchors.centerIn: parent
        text: "Hello"
        color: "white"
        font.pixelSize: 28
    }
}
