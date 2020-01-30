import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.labs.settings 1.1
import QtMultimedia 5.14

import oscope 1.0

Window {
    visible: true
    width: 1250
    height: 600
    title: qsTr("OScope")
    Rectangle {
        color: "black"
        anchors.fill: parent
    }
    Settings {
        id: settings
        property alias address: addressField.text
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
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 450
            Layout.minimumWidth: 450
            Layout.maximumWidth: 450
            color: "white"
            ColumnLayout {
                anchors.fill: parent
                ToolButton {
                    Layout.fillWidth: true
                    text: scope.isRunning ? ICON_FA_STOP : ICON_FA_PLAY
                    font.family: "FontAwesome"
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
                    text: ICON_FA_HOME
                    font.family: "FontAwesome"
                    onPressedChanged: scope.homeButtonEvent(pressed)
                }
                ToolButton {
                    Layout.fillWidth: true
                    text: ICON_FA_EJECT
                    font.family: "FontAwesome"
                }
                TextField {
                    Layout.fillWidth: true
                    id: addressField
                }
                Label {
                    Layout.fillWidth: true
                    id: discoveredLabel
                    text: "0 discovered"
                }

                ComboBox {
                    id: discoveredNodes
                    Layout.fillWidth: true
                    onModelChanged: {
                        discoveredLabel.text = count + " discovered"
                    }
                    model: discovery.discovered
                    delegate: ItemDelegate {
                        enabled: !scope.isConnected
                        width: discoveredNodes.width
                        contentItem: Text {
                            text: modelData.name
                        }
                        onClicked: addressField.text = modelData.address
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: scope.socketState
                }
                ToolButton {
                    Layout.fillWidth: true
                    text: scope.isConnected ? "Disconnect" : "Connect"
                    onClicked: if (scope.isConnected) { scope.disconnectFromScope() } else { scope.connectToScope(addressField.text) }
                }
            }
        }
    }
}
