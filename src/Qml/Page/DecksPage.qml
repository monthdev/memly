import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    id: root
    objectName: "decksPage"

    required property color pageColor

    background: Rectangle {
        color: root.pageColor
    }

    footer: PageFooterComponent {
        centerSlot: Component {
            RoundButton {
                text: "Create Deck"
                width: 100
                height: 26
                radius: 20
                palette.buttonText: "white"

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
