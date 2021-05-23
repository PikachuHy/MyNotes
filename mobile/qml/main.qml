import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 320
    height: 480
    visible: true
    title: qsTr("MyNotes")
    property var pageStack: []
    Connections {
        target: $KeyFilter
        function onKeyBackPress(){
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

    ListView {
        id: mainListView
        width: parent.width
        height: parent.height

        model: // $Model
               ListModel {
            id: model2
        }

        delegate: Rectangle {
            width: parent.width
            height: 40
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(model.name)
                    console.log(model.icon)
                    mainListView.visible = false
                    var NoteListViewPage = Qt.createComponent(
                                "NoteListView.qml").createObject(root, {
                                                                     x: 0,
                                                                     y: 0,
                                                                     width: root.width,
                                                                     height: root.height,
                                                                     pathId: 0
                                                                 })
                    pageStack.push(NoteListViewPage)

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
