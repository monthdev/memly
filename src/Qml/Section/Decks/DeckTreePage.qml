import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "deckTreePage"

    required property color pageColor

    DeckTreeController {
        id: deckTreeController
    }

    StackView.onActivated: deckTreeController.OnActivated()
    StackView.onDeactivated: deckTreeController.OnDeactivated()

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
