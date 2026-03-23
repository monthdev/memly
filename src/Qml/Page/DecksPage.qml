import QtQuick
import QtQuick.Controls.Basic
import Memly

Page {
    background: Rectangle {
        color: ApplicationWindow.window.color
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
