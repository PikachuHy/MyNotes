import QtQuick 2.15
import cn.net.pikachu.control 1.0

Item {
    id: noteListView
    property int pathId: 0
    ListView {
        anchors.fill: parent
        visible: true
        model: ListModel {
            id: model2
        }
        delegate: NoteListItem {
            width: noteListView.width
            name: model.name
            icon: model.icon
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    noteListView.visible = false
                    console.log(model.name)
                    console.log(model.icon)
                    if (model.type === 'note') {
                        console.log('load note', model.path)
                        console.log(model.basePath)
                        var NoteViewPage = Qt.createComponent(
                                    "NoteView.qml").createObject(root, {
                                                                     "x": 0,
                                                                     "y": noteListView.y,
                                                                     "width": noteListView.width,
                                                                     "height": noteListView.height,
                                                                     "source": model.path,
                                                                     "path": model.path
                                                                 })
                        root.pageStack.push(NoteViewPage)
                    } else {
                        var NoteListViewPage = Qt.createComponent(
                                    "NoteListView.qml").createObject(root, {
                                                                         "x": 0,
                                                                         "y": noteListView.y,
                                                                         "width": noteListView.width,
                                                                         "height": noteListView.height,
                                                                         "pathId": model.pathId
                                                                     })
                        root.pageStack.push(NoteListViewPage)
                    }
                }
            }
        }
    }
    Text {
        id: returnBtn
        visible: false
        anchors.centerIn: parent
        text: "Return"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                noteListView.destroy()
                root.pageStack.pop()
                pageStack[pageStack.length - 1].visible = true
            }
        }
    }

    Component.onCompleted: {
        console.log(pathId)
        const pathList = $DbManager.getPathList_qml(pathId)
        const noteList = $DbManager.getNoteList_qml(pathId)
        if (pathList.length + noteList.length === 0) {
            console.log('empty path')
            console.log(width, height)
            returnBtn.visible = true
        }

        pathList.forEach(data => {
                             //                             console.log(data.name)
                             model2.append({
                                               "name": data.name,
                                               "icon": "qrc" + data.icon,
                                               "pathId": data.pathId,
                                               "type": "folder"
                                           })
                         })
        noteList.forEach(data => {
                             //                             console.log(data.name)
                             model2.append({
                                               "name": data.name,
                                               "icon": "qrc" + data.icon,
                                               "pathId": data.pathId,
                                               "path": data.path,
                                               "basePath": data.basePath,
                                               "type": "note"
                                           })
                         })
    }
}
