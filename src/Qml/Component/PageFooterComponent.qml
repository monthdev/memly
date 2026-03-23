import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    implicitHeight: 72

    property Component farLeftSlot: null
    property Component centerLeftSlot: null
    property Component centerSlot: null
    property Component centerRightSlot: null
    property Component farRightSlot: null

    Item {
        anchors.fill: parent

        Item {
            id: farLeftAnchor
            width: 120
            height: 32
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
        }

        Item {
            id: centerGroup
            width: 360
            height: 32
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
        }

        Item {
            id: centerAnchor
            width: 120
            height: 32
            anchors.centerIn: centerGroup
        }

        Item {
            id: centerLeftAnchor
            width: 120
            height: 32
            anchors.verticalCenter: centerAnchor.verticalCenter
            anchors.right: centerAnchor.left
            anchors.rightMargin: 12
        }

        Item {
            id: centerRightAnchor
            width: 120
            height: 32
            anchors.verticalCenter: centerAnchor.verticalCenter
            anchors.left: centerAnchor.right
            anchors.leftMargin: 12
        }

        Item {
            id: farRightAnchor
            width: 120
            height: 32
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
        }

        Loader {
            anchors.centerIn: farLeftAnchor
            sourceComponent: root.farLeftSlot
        }
        Loader {
            anchors.centerIn: centerLeftAnchor
            sourceComponent: root.centerLeftSlot
        }
        Loader {
            anchors.centerIn: centerAnchor
            sourceComponent: root.centerSlot
        }
        Loader {
            anchors.centerIn: centerRightAnchor
            sourceComponent: root.centerRightSlot
        }
        Loader {
            anchors.centerIn: farRightAnchor
            sourceComponent: root.farRightSlot
        }
    }
}
