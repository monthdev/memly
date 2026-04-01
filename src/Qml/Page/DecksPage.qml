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
        Row {
            anchors.centerIn: parent

            PageFooterButtonComponent {
                text: "Create Deck"
            }
        }
    }
}
