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
        id: topMenu

        onCurrentIndexChanged: {
            if (swipeView.currentIndex !== topMenu.currentIndex)
                swipeView.setCurrentIndex(topMenu.currentIndex);
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: true

        Component.onCompleted: topMenu.setCurrentIndex(swipeView.currentIndex)

        onCurrentIndexChanged: {
            if (topMenu.currentIndex !== swipeView.currentIndex)
                topMenu.setCurrentIndex(swipeView.currentIndex);
        }

        DecksSection {
            pageColor: window.color
        }

        AddCardSection {
            pageColor: window.color
        }

        BrowseSection {
            pageColor: window.color
        }
    }
}
