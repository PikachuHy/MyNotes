import QtQuick 2.15
import QtQuick.Window 2.15
import cn.net.pikachu.control 1.0
import Controller 1.0

Window {
    id: root
    width: 800
    height: 600
    title: "TreeView Example"
    visible: true

    NoteTreeView {
        width: 300
        height: parent.height
        onClicked: {
            if (controller.isNote(index)) {
                let path = controller.getNoteFullPath(index)
                console.log(path)
                editor.source = path
            }
        }
    }
    QtQuickMarkdownItem {
        id: editor
        source: "/Users/pikachu/Documents/MyNotes/workshop/jTioZzs/index.md"
        x: 305
        width: parent.width - 308
        height: parent.height
    }
    Controller {
        id: controller
    }

    Component.onCompleted: {
        editor.height = root.height
    }
}
