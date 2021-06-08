import QtQuick 2.15

Rectangle {
    implicitWidth: 200
    height: 40
    property string name: ""
    property string icon: ""
    Image {
        source: icon
        width: 32
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: name
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.verticalCenter: parent.verticalCenter
    }
}
