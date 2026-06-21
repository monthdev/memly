import QtQuick

Item {
    id: root

    implicitWidth: 600
    implicitHeight: 72
    width: parent ? parent.width : implicitWidth
    height: implicitHeight

    property int horizontalPadding: 20
    property int bottomPadding: 14
    property int contentHeight: 32

    default property alias content: contentItem.data

    Item {
        id: contentItem
        height: root.contentHeight
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: root.horizontalPadding
        anchors.rightMargin: root.horizontalPadding
        anchors.bottomMargin: root.bottomPadding
    }
}
