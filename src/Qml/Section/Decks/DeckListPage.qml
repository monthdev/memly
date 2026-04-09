import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "deckListPage"

    required property color pageColor

    DeckListController {
        id: deckListController
    }

    StackView.onActivated: deckListController.OnActivated()
    StackView.onDeactivated: deckListController.OnDeactivated()

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
