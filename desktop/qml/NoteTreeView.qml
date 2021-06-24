import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import Controller 1.0
import QtQml.Models 2.15

TreeView {
    id: treeView
    width: 400
    height: parent.height
    signal noteClicked(string path)
    TableViewColumn {
        title: "Name"
        role: "text"
        width: 400 - 2
    }
    headerVisible: false
    frameVisible: false
    selectionMode: SelectionMode.SingleSelection
    selection: ItemSelectionModel {
        id: itemSelectionModel
        model: treeView.model
    }

    rowDelegate: Rectangle {
        height: 32
        property color selectedColor: parent.activeFocus ? "transparent" : "#CDE8FF"
        color: styleData.selected ? selectedColor : "transparent"
        // color: styleData.hasActiveFocus ? "#CDE8FF" : "transparent"
        // TODO: 如何实现hover?
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
    itemDelegate: Item {
        // color: styleData.selected ? "#CDE8FF" : "transparent"
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
        MouseArea {
            anchors.fill: parent

                        acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: {
                var index = styleData.index
                itemSelectionModel.setCurrentIndex(index, 0)
                if (mouse.button === Qt.RightButton) {
                    if (controller.isNote(index)) {
                        noteMenu.popup()
                    } else {
                        menu.popup()
                    }
                } else {
                    if (controller.isNote(index)) {
                        var path = controller.getNoteFullPath(index)
                        treeView.noteClicked(path)
                    } else {
                        if (isExpanded(index)) {
                            collapse(index)
                        } else {
                            expand(index)
                        }
                    }
                }
            }
        }
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
    Menu {
        id: menu

        MenuItem {
            text: "New Note"
            onTriggered: {
                console.log('new note')
                console.log(itemSelectionModel.currentIndex)
                newNoteNameDialog.show()
            }
        }
    }
    Menu {
        id: noteMenu

        MenuItem {
            text: "Delete"
            onTriggered: {
                console.log('delete note')
                var path = controller.getNoteFullPath(
                            itemSelectionModel.currentIndex)
                console.log(path)
            }
        }
    }
    Dialog {
        id: newNoteNameDialog
        title: "New Note Name"
        TextField {
            id: newNoteNameTextField
            anchors.fill: parent

        }
        onAccepted: {

        }

        function show() {
            newNoteNameTextField.text = ""
            newNoteNameDialog.visible = true
        }
    }

    Controller {
        id: controller
    }
}
