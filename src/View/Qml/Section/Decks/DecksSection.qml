import QtQuick
import QtQuick.Controls.Basic

StackView {
    id: root

    required property color pageColor

    initialItem: DeckPage {
        pageColor: root.pageColor
    }
}
