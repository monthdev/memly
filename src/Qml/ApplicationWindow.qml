import QtQuick
import QtQuick.Controls.Basic
import QtCore

ApplicationWindow {
    id: window
    visible: true
    title: "Memly"
    color: "#121212"

    width: 800
    height: 700
    minimumWidth: 600
    minimumHeight: 90

    Settings {
        category: "ApplicationWindow"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    header: TopMenuComponent {
        onDecksClicked: stackView.replaceCurrentItem(Qt.resolvedUrl("Page/DecksPage.qml"))
        onAddClicked: {}
        onBrowseClicked: {}
        onJournalClicked: {}
        onInfoClicked: {}
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: DecksPage {}
    }
}
