import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls 2.15 as Ctrl2
import FishUI 1.0 as FishUI
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

        MenuItem {
            text: "New Folder"
            onTriggered: {

            }
        }
    }
    Menu {
        id: noteMenu

        MenuItem {
            text: "Trash Note"
            onTriggered: {
                console.log('transh note')
                controller.trashNote(itemSelectionModel.currentIndex);
            }
        }
    }
    InputDialog {
        id: newNoteNameDialog
        title: "New Note Name"
        placeholderText: "Please input new note name"

        onAccepted: {
            var noteName = newNoteNameDialog.text
            if (noteName.length === 0) {
                root.showPassiveNotification('note name cannot be empty', 800)
                return
            }

            var pathId = controller.getPathId(itemSelectionModel.currentIndex)
            var newIndex = controller.createNewNote(itemSelectionModel.currentIndex, noteName)

            itemSelectionModel.setCurrentIndex(newIndex, 0)
            var path = controller.getNoteFullPath(newIndex)
            treeView.noteClicked(path)
        }
    }

    NoteController {
        id: controller
    }
}
