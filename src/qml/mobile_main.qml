import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import cn.net.pikachu.control 1.0

ApplicationWindow {
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

        Profile {
            id: mainListView
            anchors.fill: parent
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
                }
            }
        }

        Text {
            text: qsTr("MyNotes")
            anchors.centerIn: parent
        }
    }



    // 不显示调试信息
    Column {
        visible: false
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

    function pushPageToStack(qmlFileName) {
        if (pageStack.length > 0) {
            pageStack[pageStack.length - 1].visible = false
        }

        var page = Qt.createComponent(qmlFileName).createObject(
                    listContainer, {
                        "x": 0,
                        "y": 0,
                        "width": listContainer.width,
                        "height": listContainer.height
                    })
        pageStack.push(page)
        if (pageStack.length > 1) {
            returnIcon.visible = true
        }
    }

    function popPageInStack() {
        if (pageStack.length > 0) {
            pageStack.pop().destroy()
            pageStack[pageStack.length - 1].visible = true
        }

        if (pageStack.length == 1) {
            returnIcon.visible = false
        }
    }

    Rectangle {
        id: dlgBg
        color: "transparent"
        anchors.fill: parent
        visible: false
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
    }
}
