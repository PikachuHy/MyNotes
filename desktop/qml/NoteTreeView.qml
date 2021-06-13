import QtQuick 2.15
import QtQuick.Controls 1.4

TreeView {
    id: treeView
    width: 400
    height: parent.height

    TableViewColumn {
        title: "Name"
        role: "text"
        width: 400 - 2
    }
    headerVisible: true
    frameVisible: false
    rowDelegate: Rectangle {
        height: 32
         property color selectedColor: parent.activeFocus ? "transparent" : "#CDE8FF"
         color: styleData.selected ? selectedColor : "transparent"
        // color: styleData.hasActiveFocus ? "#CDE8FF" : "transparent"
    }
    itemDelegate: Item {
        Row {
            width: parent.width - 10
            spacing: 10
            Image {
                width: 32
                height: 32
                source: model === null ? "" : "qrc://" + model.iconPath
            }

            Text {
                text: model === null ? "" : model.text
                anchors.verticalCenter: parent.verticalCenter
            }
        }
//        MouseArea {
//            anchors.fill: parent
//            hoverEnabled: true
//            onEntered: {
//                parent.color = "#CDE8FF"
//            }
//            onExited: {
//                parent.color = "transparent"
//            }
//        }
    }

    model: treeModel
    onDoubleClicked: {
        if (isExpanded(index)) {
            collapse(index)
        } else {
            expand(index)
        }
    }
//    onClicked: {
//        console.log(index)
//        console.log(index.row, index.column, index.internalId)
//        treeView.clicked(index)
//    }
}
