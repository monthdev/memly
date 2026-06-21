import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "deckPage"

    required property color pageColor

    DeckPageController {
        id: deckPageController
    }

    background: Rectangle {
        color: root.pageColor
    }

    footer: PageFooterComponent {
        Row {
            anchors.centerIn: parent

            PageFooterButtonComponent {
                text: "Create Deck"
            }
        }
    }
}
