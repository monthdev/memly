import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "decksPage"

    required property color pageColor

    StackView.onActivated: DeckListBridge.onDecksPageActivated()
    StackView.onDeactivated: DeckListBridge.onDecksPageDeactivated()

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
