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

    StackView.onActivated: deckTreeTableController.OnActivated()
    StackView.onDeactivated: deckTreeTableController.OnDeactivated()

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
