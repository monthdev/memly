import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "deckTreePage"

    required property color pageColor

    DeckTreeTableController {
        id: deckTreeTableController
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
