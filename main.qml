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
    CustomVideoSurface {
        id: surface
    }
    Stream {
        id: stream
        surface: surface.videoSurface
    }
    RowLayout {
        anchors.fill: parent
        VideoOutput {
            id: videoOut
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 600
            Layout.minimumWidth: 800
            source: surface
            visible: true
        }
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: 200
            Layout.minimumWidth: 200
            Layout.maximumWidth: 200
            ToolButton {
                text: "RUN"
            }
            ToolButton {
                text: "SEQ"
            }
            ToolButton {
                text: "AUTO"
            }
            ToolButton {
                text: "50%"
            }
            ToolButton {
                text: "HOME"
            }
            ToolButton {
                text: "UP"
            }
            TextInput {
                id: address
                text: "192.168.1.251"
            }
            Label {
                text: stream.socketState
            }
            ToolButton {
                text: stream.isConnected ? "Disconnect" : "Connect"
                onClicked: if (stream.isConnected) { stream.disconnectFromScope() } else { stream.connectToScope(address.text) }
            }
        }
    }
}
