import QtQuick 2.15
import QtQuick.Controls 2.15
import Controller 1.0

TreeView {
    id: treeView
    signal noteClicked(string path)
    model2: treeModel
    onItemClicked: index => {
                       if (controller.isNote(index)) {
                           treeView.noteClicked(
                               controller.getNoteFullPath(index))
                       }
                   }
    onItemRightClicked: index => {

                            if (controller.isNote(index)) {
                                noteMenu.popup()
                            } else {
                                menu.popup()
                            }
                        }

    Menu {
        id: menu

        MenuItem {
            text: "New Note"
            onTriggered: {
                console.log('new note')
                console.log(treeView.currentIndex)
                newNoteNameDialog.show()
            }
        }

        MenuItem {
            text: "New Folder"
            onTriggered: {
                newFolderNameDialog.show()
            }
        }

        MenuItem {
            text: "Trash Folder"
            onTriggered: {
                controller.trashFolder(treeView.currentIndex)
            }
        }
    }
    Menu {
        id: noteMenu

        MenuItem {
            text: "Trash Note"
            onTriggered: {
                console.log('transh note')
                controller.trashNote(treeView.currentIndex)
            }
        }

        MenuItem {
            text: "Open In Typora"
            onTriggered: {
                treeView.openNoteInTypora()
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

            var pathId = controller.getPathId(treeView.currentIndex)
            var newIndex = controller.createNewNote(treeView.currentIndex, noteName)

            treeView.setCurrentIndex(newIndex, 0)
            var path = controller.getNoteFullPath(newIndex)
            treeView.noteClicked(path)
        }
    }

    InputDialog {
        id: newFolderNameDialog
        title: "New Note Name"
        placeholderText: "Please input new note name"

        onAccepted: {
            var folderName = newFolderNameDialog.text
            if (folderName.length === 0) {
                root.showPassiveNotification('folder name cannot be empty', 800)
                return
            }

            var newIndex = controller.createNewFolder(
                        treeView.currentIndex, folderName)

            treeView.setCurrentIndex(newIndex, 0)
        }
    }
    function openNoteInTypora() {
            var index = treeView.currentIndex
            if (controller.isNote(index)) {
                var path = controller.getNoteFullPath(index)
                console.log('path', path)
                controller.openInTypora(path)
            }
        }
    NoteController {
        id: controller
    }
}
