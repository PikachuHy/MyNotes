import QtQuick 2.15

Rectangle {
    id: item
    width: 64
    height: 64
    property alias iconSource: icon.source
    signal clicked()
    Image {
        id: icon
        width: 32
        height: 32
        anchors.centerIn: parent
        source: "qrc:/icon/settings_64x64.png"
    }
    MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {

            item.clicked()
        }

        onEntered: {
            item.color = "#eee"
            cursorShape = Qt.PointingHandCursor
        }
        onExited: {
            item.color = "transparent"
            cursorShape = Qt.ArrowCursor
        }
    }
}
