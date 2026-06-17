import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "deckTreeSnapshotPage"

    required property color pageColor

    DeckTreeSnapshotController {
        id: deckTreeSnapshotController
    }

    DeckController {
        id: deckController
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
