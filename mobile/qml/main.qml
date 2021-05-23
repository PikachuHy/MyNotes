import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 320
    height: 480
    visible: true
    title: qsTr("MyNotes")
    property var pageStack: []
    ListView {
        id: mainListView
        width: 180
        height: parent.height

        model: // $Model
               ListModel {
            id: model2
        }

        delegate: Rectangle {
            width: parent.width
            height: 64
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(model.name)
                    console.log(model.icon)
                    mainListView.visible = false
                    pageStack.push(mainListView)
                    var NoteListViewPage = Qt.createComponent(
                                "NoteListView.qml").createObject(root, {
                                                                     x: 0,
                                                                     y: 0,
                                                                     width: root.width,
                                                                     height: root.height,
                                                                     pathId: 0
                                                                 })
                }
            }
            Row {

                Image {
                    source: model.icon
                    width: 32
                    height: 32
                }
                Text {
                    text: model.name
                }
            }
        }
    }
    Component.onCompleted: {
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
