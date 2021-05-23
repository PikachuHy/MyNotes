import QtQuick 2.0
Item {
    id: noteListView
    property int pathId: 0
    // property array modelStack: []
    ListView {
        anchors.fill: parent
        model: ListModel {
            id: model2
        }
        delegate: Rectangle {
            width: noteListView.width
            height: 40
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    noteListView.visible = false
                    console.log(model.name)
                    console.log(model.icon)
                    // modelStack.append(model2)

                    var NoteListViewPage = Qt.createComponent(
                                "NoteListView.qml").createObject(root, {
                                                                     "x": 0,
                                                                     "y": 0,
                                                                     "width": noteListView.width,
                                                                     "height": noteListView.height,
                                                                     "pathId": model.pathId
                                                                 })
                    root.pageStack.push(NoteListViewPage)

                }
            }
            Image {
                source: model.icon
                width: 32
                height: 32
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: model.name
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.verticalCenter: parent.verticalCenter

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
                             console.log(data.name)
                             model2.append({
                                               "name": data.name,
                                               "icon": "qrc" + data.icon,
                                               "pathId": data.pathId
                                           })
                         })
        noteList.forEach(data => {
                             console.log(data.name)
                             model2.append({
                                               "name": data.name,
                                               "icon": "qrc" + data.icon,
                                               "pathId": data.pathId
                                           })
                         })
    }
}
