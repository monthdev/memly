import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "decksPage"

    required property color pageColor

    DeckListController {
        id: deckListController
    }

    StackView.onActivated: deckListController.onActivated()
    StackView.onDeactivated: deckListController.onDeactivated()

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
