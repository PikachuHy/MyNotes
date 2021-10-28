import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import cn.net.pikachu.control 1.0

Window {
    id: root
    width: 320
    height: 480
    visible: true
    title: qsTr("MyNotes")
    property var pageStack: []
    property int titleHeight: 32

    Connections {
        target: $KeyFilter
        function onKeyBackPress() {
            console.log('back press')
            if (pageStack.length === 1) {
                console.log('no page to destroy')
                Qt.quit()
            } else {
                popPageInStack()
            }
        }
    }
    Rectangle {
        id: listContainer
        y: titleHeight
        width: parent.width
        height: parent.height - titleHeight

        ListView {
            id: mainListView
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
                        if (model.name === "Workshop") {
                            mainListView.visible = false
                            var NoteListViewPage = Qt.createComponent(
                                        "NoteListView.qml").createObject(
                                        listContainer, {
                                            "x": 0,
                                            "y": 0,
                                            "width": listContainer.width,
                                            "height": listContainer.height,
                                            "pathId": 0
                                        })
                            pageStack.push(NoteListViewPage)
                            returnIcon.visible = true
                            avatorIcon.visible = false
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: title
        width: parent.width
        height: titleHeight
        color: "#fff"

        MouseArea {
            width: parent.width / 3
            height: parent.height
            anchors.left: parent.left
            Image {
                id: avatorIcon
                width: 32
                height: 32
                source: "qrc:/icon/avatar_300x300.png"
                anchors.left: parent.left
            }
            Image {
                id: returnIcon
                visible: false
                width: 32
                height: 32
                source: "qrc:/icon/return_64x64.png"
                anchors.left: parent.left
            }
            onClicked: {
                if (returnIcon.visible) {
                    console.log('return')
                    if (pageStack.length === 1) {
                        console.log('no page to destroy')
                    } else {
                        popPageInStack()
                    }
                } else {
                    mainListView.visible = false
                    var page = Qt.createComponent("Profile.qml").createObject(
                                listContainer, {
                                    "x": 0,
                                    "y": 0,
                                    "width": listContainer.width,
                                    "height": listContainer.height
                                })
                    pageStack.push(page)
                    returnIcon.visible = true
                    avatorIcon.visible = false
                }
            }
        }

        Text {
            text: qsTr("MyNotes")
            anchors.centerIn: parent
        }

        MouseArea {
            width: parent.width / 3
            height: parent.height
            anchors.right: parent.right
            Image {
                width: 32
                height: 32
                source: "qrc:/icon/settings_64x64.png"
                anchors.right: parent.right
            }
            onClicked: {


                /*
                // 无法修改，先注释掉
                console.log('choose folder')
                mainListView.visible = false
                var FileListViewPage = Qt.createComponent(
                            "FileListView.qml").createObject(listContainer, {
                                                                 "x": 0,
                                                                 "y": 0,
                                                                 "width": listContainer.width,
                                                                 "height": listContainer.height,
                                                                 "path": $FileSystem.defaultPath()
                                                             })
                pageStack.push(FileListViewPage)
                */
//                dlgBg.visible = true
//                settingDialog.visible = true

                mainListView.visible = false
                var page = Qt.createComponent("SettingView.qml").createObject(
                            listContainer, {
                                "x": 0,
                                "y": 0,
                                "width": listContainer.width,
                                "height": listContainer.height
                            })
                pageStack.push(page)
                returnIcon.visible = true
                avatorIcon.visible = false
            }
        }
    }


    /*
    // 不显示调试信息
    Column {
        anchors.bottom: parent.bottom
        Text {
            text: qsTr("MANAGE_EXTERNAL_STORAGE: ") + $Controller.hasManageExternalStorage();
        }

        Text {
            text: qsTr("WRITE_EXTERNAL_STORAGE: ") + $Controller.hasWriteExternalStoragePermission()
        }
        Text {
            text: $Controller.noteDataPath()
        }
        Text {
            text: $Controller.configStorePath()
        }
    }
    */
    function popPageInStack() {
        pageStack.pop().destroy()
        pageStack[pageStack.length - 1].visible = true
        if (pageStack.length == 1) {
            returnIcon.visible = false
            avatorIcon.visible = true
        }
    }

    Rectangle {
        id: dlgBg
        color: "transparent"
        anchors.fill: parent
        visible: false
    }

    Row {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        SideMenu {
            id: musicPlayer
            iconSource: "qrc:/icon/music_64x64.png"
            onClicked: {
                mainListView.visible = false
                var page = Qt.createComponent("MusicPlayer.qml").createObject(
                            listContainer, {
                                "x": 0,
                                "y": 0,
                                "width": listContainer.width,
                                "height": listContainer.height
                            })
                pageStack.push(page)
                returnIcon.visible = true
                avatorIcon.visible = false
                musicPlayer.visible = false
            }
        }

        SideMenu {
            id: webBrowser
            iconSource: "qrc:/icon/browser_64x64.png"
            onClicked: {
                mainListView.visible = false
                var page = Qt.createComponent("WebBrowser.qml").createObject(
                            listContainer, {
                                "x": 0,
                                "y": 0,
                                "width": listContainer.width,
                                "height": listContainer.height
                            })
                pageStack.push(page)
                returnIcon.visible = true
                avatorIcon.visible = false
                musicPlayer.visible = false
                webBrowser.visible = false
            }
        }
    }

    SettingDialog {
        id: settingDialog
        visible: false
        onRejected: {
            dlgBg.visible = false
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
