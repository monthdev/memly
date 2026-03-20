import QtQuick
import QtQuick.Controls.Basic
import QtCore

ApplicationWindow {
    id: window
    visible: true
    title: "Memly"
    color: "#121212"

    width: 800
    height: 700
    minimumWidth: 400
    minimumHeight: 90

    Settings {
        category: "MainWindow"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#3a3a3a"
        z: 10
    }

    Item {
        id: topMenuContainer
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        width: 298
        height: 30

        property int hoveredIndex: -1
        property int pressedIndex: -1

        Canvas {
            id: topMenuCanvas
            anchors.fill: parent
            antialiasing: true

            onPaint: {
                const ctx = getContext("2d");
                const stroke = "#2b2b2b";
                const normalFill = "#1f1f1f";
                const hoverFill = "#262626";
                const pressedFill = "#2c2c2c";
                const radius = 12;
                const buttonWidth = 100;
                const buttonHeight = height;

                ctx.reset();
                ctx.clearRect(0, 0, width, height);

                function fillFor(index) {
                    if (topMenuContainer.pressedIndex === index)
                        return pressedFill;
                    if (topMenuContainer.hoveredIndex === index)
                        return hoverFill;
                    return normalFill;
                }

                function drawLeftButton(x, w, h, r, fill) {
                    ctx.beginPath();
                    ctx.moveTo(x, 0);
                    ctx.lineTo(x + w, 0);
                    ctx.lineTo(x + w, h);
                    ctx.lineTo(x + r, h);
                    ctx.quadraticCurveTo(x, h, x, h - r);
                    ctx.lineTo(x, 0);
                    ctx.closePath();
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                function drawMiddleButton(x, w, h, fill) {
                    ctx.beginPath();
                    ctx.rect(x, 0, w, h);
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                function drawRightButton(x, w, h, r, fill) {
                    ctx.beginPath();
                    ctx.moveTo(x, 0);
                    ctx.lineTo(x + w, 0);
                    ctx.lineTo(x + w, h - r);
                    ctx.quadraticCurveTo(x + w, h, x + w - r, h);
                    ctx.lineTo(x, h);
                    ctx.lineTo(x, 0);
                    ctx.closePath();
                    ctx.fillStyle = fill;
                    ctx.fill();
                }

                drawLeftButton(0, buttonWidth, buttonHeight, radius, fillFor(0));
                drawMiddleButton(99, buttonWidth, buttonHeight, fillFor(1));
                drawRightButton(198, buttonWidth, buttonHeight, radius, fillFor(2));

                ctx.strokeStyle = stroke;
                ctx.lineWidth = 1;

                ctx.beginPath();
                // ctx.moveTo(0.5, 0.5);
                // ctx.lineTo(width - 0.5, 0.5);
                ctx.moveTo(0.5, 0.5);
                ctx.lineTo(0.5, buttonHeight - radius - 0.5);
                ctx.quadraticCurveTo(0.5, buttonHeight - 0.5, radius + 0.5, buttonHeight - 0.5);
                ctx.moveTo(width - 0.5, 0.5);
                ctx.lineTo(width - 0.5, buttonHeight - radius - 0.5);
                ctx.quadraticCurveTo(width - 0.5, buttonHeight - 0.5, width - radius - 0.5, buttonHeight - 0.5);
                ctx.moveTo(radius + 0.5, buttonHeight - 0.5);
                ctx.lineTo(width - radius - 0.5, buttonHeight - 0.5);
                ctx.stroke();
            }
        }

        Repeater {
            model: [
                {
                    label: "Decks",
                    x: 0
                },
                {
                    label: "Add Card",
                    x: 99
                },
                {
                    label: "Browse",
                    x: 198
                }
            ]

            delegate: Item {
                x: modelData.x
                y: 0
                width: 100
                height: topMenuContainer.height

                Text {
                    anchors.centerIn: parent
                    text: modelData.label
                    color: "white"
                    font.pixelSize: 15
                    font.bold: true
                }

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                    onHoveredChanged: {
                        topMenuContainer.hoveredIndex = hovered ? index : (topMenuContainer.hoveredIndex === index ? -1 : topMenuContainer.hoveredIndex);
                        topMenuCanvas.requestPaint();
                    }
                }

                TapHandler {
                    onPressedChanged: {
                        topMenuContainer.pressedIndex = pressed ? index : (topMenuContainer.pressedIndex === index ? -1 : topMenuContainer.pressedIndex);
                        topMenuCanvas.requestPaint();
                    }
                }
            }
        }
    }







    RoundButton {
        id: createDeckButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 17

        text: "Create Deck"
        palette.buttonText: "white"
        width: 100
        height: 26
        radius: 20
        z: 1

        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }
}
