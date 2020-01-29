import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 5.14

import oscope 1.0

Window {
    visible: true
    width: 740
    height: 480
    title: qsTr("OScope")
    Stream {
        id: stream
    }
    RowLayout {
        anchors.fill: parent
        VideoOutput {
            id: videoOut
            Layout.fillHeight: true
            Layout.fillWidth: true
            source: stream.mediaPlayer
        }
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: 150
            Layout.minimumWidth: 150
            Layout.maximumWidth: 150
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
                text: "10.99.98.15"
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
