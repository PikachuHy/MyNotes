import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
import cn.net.pikachu.control 1.0

Item {
    id: fileListView
    width: 300
    property string path
    property string type: "FileListView"
    ListView {
        anchors.fill: parent
        visible: true
        model: ListModel {
            id: model2
        }

        delegate:
            NoteListItem {
            width: fileListView.width
            name: model.name
            icon: model.icon
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileListView.visible = false
                    console.log(model.name)
                    console.log(model.icon)
                    console.log('load dir:', model.path)
                    var fileListViewPage = Qt.createComponent(
                                "FileListView.qml").createObject(listContainer, {
                                                                     "x": 0,
                                                                     "y": 0,
                                                                     "width": listContainer.width,
                                                                     "height": listContainer.height,
                                                                     "path": model.path
                                                                 })
                    root.pageStack.push(fileListViewPage)
                }
            }
        }

    }
    function exitFileListView() {
        while (root.pageStack[root.pageStack.length-1].type && root.pageStack[root.pageStack.length-1].type === type) {
            pageStack.pop().destroy()
        }
        pageStack[pageStack.length - 1].visible = true
    }

    Image {
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 20
        source: "qrc:/icon/check_64x64.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log('change data dir to: ', path)
                $Controller.setNoteDataPath(path)
                dialog.open()
            }
        }
    }

    Dialog {
        id: dialog
        title: qsTr("修改配置")
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        Text {
            text: qsTr("重启后配置生效，立马重启？")
        }
        onAccepted: {
            console.log('exit')
            Qt.exit(0)
        }

        onRejected: {
            exitFileListView()
        }
    }

    Component.onCompleted: {
        console.log(path)
        const pathList = $FileSystem.listDir(path)
        pathList.forEach(data => {
                             const fullPath = path + '/' + data
                             const type = $FileSystem.isDir(fullPath) ? "folder" : "file";
                             console.log(fullPath, $FileSystem.isDir(fullPath))
                             if ($FileSystem.isDir(fullPath)) {
                                 model2.append({
                                                   "name": data,
                                                   "icon": "qrc:/icon/folder_64x64.png",
                                                   "path": fullPath,
                                                   "type": "folder"
                                               })
                             }


                         })
        console.log(pathList)
        console.log(model2)
    }
}
