import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 5.14

import oscope 1.0

Window {
    visible: true
    width: 1000
    height: 600
    title: qsTr("OScope")
    Rectangle {
        color: "black"
        anchors.fill: parent
    }

    CustomVideoSurface {
        id: surface
    }
    Scope {
        id: scope
        surface: surface.videoSurface
    }
    RowLayout {
        anchors.fill: parent
        MouseArea {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 600
            Layout.minimumWidth: 800

            VideoOutput {
                anchors.fill: parent
                id: videoOut
                source: surface
                visible: true
                fillMode: VideoOutput.Stretch
            }

            onPressedChanged: {
                var rx = mouseX * 800 / width;
                var ry = mouseY * 600 / height;
                scope.mouseEvent(rx, ry, pressed);
            }
        }
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: 200
            Layout.minimumWidth: 200
            Layout.maximumWidth: 200
            ToolButton {
                Layout.fillWidth: true
                text: scope.isRunning ? "STOP" : "RUN"
                onPressedChanged: scope.runButtonEvent(pressed)
            }
            ToolButton {
                Layout.fillWidth: true
                text: "SEQ"
                onPressedChanged: scope.singleButtonEvent(pressed)
            }
            ToolButton {
                Layout.fillWidth: true
                text: "AUTO"
                onPressedChanged: scope.autoButtonEvent(pressed)
            }
            ToolButton {
                Layout.fillWidth: true
                text: "50%"
                onPressedChanged: scope.halfButtonEvent(pressed)
            }
            ToolButton {
                Layout.fillWidth: true
                text: "HOME"
                onPressedChanged: scope.homeButtonEvent(pressed)
            }
            ToolButton {
                Layout.fillWidth: true
                text: "UP"
            }
            TextField {
                Layout.fillWidth: true
                id: addressField
                text: settings.value("address", "192.168.1.21")
                onTextEdited: this.address = text
            }
            Label {
                Layout.fillWidth: true
                text: scope.socketState
            }
            ToolButton {
                Layout.fillWidth: true
                text: scope.isConnected ? "Disconnect" : "Connect"
                onClicked: if (scope.isConnected) { scope.disconnectFromScope() } else { scope.connectToScope(addressField) }
            }
        }
    }
}
