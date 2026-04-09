pragma ComponentBehavior: Bound
import QtQuick

Item {
    id: root

    implicitWidth: 400
    implicitHeight: 31

    width: parent ? parent.width : implicitWidth
    height: implicitHeight

    property int currentIndex: 0
    property int hoveredIndex: -1
    property int pressedIndex: -1

    onCurrentIndexChanged: topMenuCanvas.requestPaint()

    function setCurrentIndex(index) {
        root.currentIndex = index;
    }

    readonly property var menuModel: ["Decks", "Add Card", "Browse"]

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        z: 10
        color: "#3a3a3a"
    }

    Item {
        id: menuContainer
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(root.width, root.implicitWidth)
        height: root.height - 1

        readonly property int buttonCount: root.menuModel.length
        readonly property real buttonWidth: width / buttonCount
        readonly property real buttonHeight: height
        readonly property real radius: 12

        Canvas {
            id: topMenuCanvas
            anchors.fill: parent
            antialiasing: true

            onPaint: {
                const ctx = getContext("2d");
                const stroke = "#2b2b2b";
                const normalFill = "#1f1f1f";
                const activeFill = "#2e2e2e";
                const hoverFill = "#232323";
                const pressedFill = "#3f3f3f";
                const fillOverlap = 1;

                ctx.reset();
                ctx.clearRect(0, 0, width, height);

                function fillFor(index) {
                    if (root.pressedIndex === index)
                        return pressedFill;
                    if (root.currentIndex === index)
                        return activeFill;
                    if (root.hoveredIndex === index)
                        return hoverFill;
                    return normalFill;
                }

                function drawLeftButton(x, w, h, r, fill) {
                    ctx.beginPath();
                    ctx.moveTo(x, 0);
                    ctx.lineTo(x + w + fillOverlap, 0);
                    ctx.lineTo(x + w + fillOverlap, h);
                    ctx.lineTo(x + r, h);
                    ctx.quadraticCurveTo(x, h, x, h - r);
                    ctx.lineTo(x, 0);
                    ctx.closePath();
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                function drawMiddleButton(x, w, h, fill) {
                    ctx.beginPath();
                    ctx.rect(x - fillOverlap, 0, w + (fillOverlap * 2), h);
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                function drawRightButton(x, w, h, r, fill) {
                    ctx.beginPath();
                    ctx.moveTo(x - fillOverlap, 0);
                    ctx.lineTo(x + w, 0);
                    ctx.lineTo(x + w, h - r);
                    ctx.quadraticCurveTo(x + w, h, x + w - r, h);
                    ctx.lineTo(x - fillOverlap, h);
                    ctx.lineTo(x - fillOverlap, 0);
                    ctx.closePath();
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                for (let i = 0; i < menuContainer.buttonCount; ++i) {
                    const x = i * menuContainer.buttonWidth;
                    const fill = fillFor(i);

                    if (i === 0)
                        drawLeftButton(x, menuContainer.buttonWidth, menuContainer.buttonHeight, menuContainer.radius, fill);
                    else if (i === menuContainer.buttonCount - 1)
                        drawRightButton(x, menuContainer.buttonWidth, menuContainer.buttonHeight, menuContainer.radius, fill);
                    else
                        drawMiddleButton(x, menuContainer.buttonWidth, menuContainer.buttonHeight, fill);
                }

                ctx.strokeStyle = stroke;
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.moveTo(0.5, 0.5);
                ctx.lineTo(0.5, menuContainer.buttonHeight - menuContainer.radius - 0.5);
                ctx.quadraticCurveTo(0.5, menuContainer.buttonHeight - 0.5, menuContainer.radius + 0.5, menuContainer.buttonHeight - 0.5);
                ctx.moveTo(width - 0.5, 0.5);
                ctx.lineTo(width - 0.5, menuContainer.buttonHeight - menuContainer.radius - 0.5);
                ctx.quadraticCurveTo(width - 0.5, menuContainer.buttonHeight - 0.5, width - menuContainer.radius - 0.5, menuContainer.buttonHeight - 0.5);
                ctx.moveTo(menuContainer.radius + 0.5, menuContainer.buttonHeight - 0.5);
                ctx.lineTo(width - menuContainer.radius - 0.5, menuContainer.buttonHeight - 0.5);
                ctx.stroke();
            }
        }

        Repeater {
            model: root.menuModel

            delegate: Item {
                id: menuButton
                required property int index
                required property var modelData

                x: index * menuContainer.buttonWidth
                y: 0
                width: menuContainer.buttonWidth
                height: menuContainer.buttonHeight

                Text {
                    anchors.centerIn: parent
                    text: menuButton.modelData
                    color: "white"
                    font.pixelSize: 15
                    font.bold: true
                }

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor

                    onHoveredChanged: {
                        root.hoveredIndex = hovered ? menuButton.index : (root.hoveredIndex === menuButton.index ? -1 : root.hoveredIndex);
                        topMenuCanvas.requestPaint();
                    }
                }

                TapHandler {
                    onPressedChanged: {
                        root.pressedIndex = pressed ? menuButton.index : (root.pressedIndex === menuButton.index ? -1 : root.pressedIndex);
                        topMenuCanvas.requestPaint();
                    }

                    onTapped: root.setCurrentIndex(menuButton.index)
                }
            }
        }
    }
}
