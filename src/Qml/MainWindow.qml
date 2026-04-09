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
    minimumWidth: 400
    minimumHeight: 90

    Settings {
        category: "ApplicationWindow"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    header: TopMenuComponent {
        currentIndex: swipeView.currentIndex

        onDecksClicked: swipeView.setCurrentIndex(0)
        onAddClicked: swipeView.setCurrentIndex(1)
        onBrowseClicked: swipeView.setCurrentIndex(2)
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        DecksPage {
            pageColor: window.color
        }

        AddCardPage {
            pageColor: window.color
        }

        BrowsePage {
            pageColor: window.color
        }
    }
}
