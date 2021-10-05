import QtQuick 2.15
import QtQuick.Window 2.15
import cn.net.pikachu.control 1.0

Window {
    id: root
    width: 320
    height: 480
    visible: true
    title: qsTr("MyNotes")
    property var pageStack: []

    Connections {
        target: $KeyFilter
        function onKeyBackPress() {
            console.log('back press')
            if (pageStack.length === 1) {
                console.log('no page to destroy')
                Qt.quit()
            } else {
                pageStack.pop().destroy()
                pageStack[pageStack.length - 1].visible = true
            }
        }
    }
    Rectangle {
        id: title
        width: parent.width
        height: 32

        MouseArea {
            width: parent.width / 3
            height: parent.height
            anchors.left: parent.left
            Image {
                width: 32
                height: 32
                source: "qrc:/icon/return_64x64.png"
                anchors.left: parent.left
            }
            onClicked: {
                console.log('return')
                if (pageStack.length === 1) {
                    console.log('no page to destroy')
                } else {
                    pageStack.pop().destroy()
                    pageStack[pageStack.length - 1].visible = true
                }
            }
        }

        Text {
            text: qsTr("MyNotes")
            anchors.centerIn: parent
        }
    }

    ListView {
        id: mainListView
        y: title.height
        width: parent.width
        height: parent.height

        model: ListModel {
            id: model2
        }

        delegate: NoteListItem {
            width: root.width
            name: model.name
            icon: model.icon
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(model.name)
                    console.log(model.icon)
                    mainListView.visible = false
                    var NoteListViewPage = Qt.createComponent(
                                "NoteListView.qml").createObject(root, {
                                                                     "x": 0,
                                                                     "y": mainListView.y,
                                                                     "width": root.width,
                                                                     "height": root.height,
                                                                     "pathId": 0
                                                                 })
                    pageStack.push(NoteListViewPage)
                }
            }
        }
    }

    Component.onCompleted: {
        pageStack.push(mainListView)
        var modelData = [{
                             "name": "Workshop",
                             "icon": "qrc:/icon/workshop_64x64.png"
                         }, {
                             "name": "Watching",
                             "icon": "qrc:/icon/watching_64x64.png"
                         }, {
                             "name": "Attachment",
                             "icon": "qrc:/icon/attachment_64x64.png"
                         }, {
                             "name": "Trash",
                             "icon": "qrc:/icon/trash_64x64.png"
                         }]
        modelData.forEach(it => {
                              model2.append(it)
                          })
    }
}
