import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
Pane {
    id: pane
    width: 800
    height: 400
    Column {
        width: parent.width
        spacing: 20
        Image {
            id: avatar
            anchors.horizontalCenter: parent.horizontalCenter
            width: 120
            height: 120
            source: "qrc:/icon/avatar_300x300.png"
        }
        Label {
            id: title
            anchors.horizontalCenter: parent.horizontalCenter
            text: "大黄老鼠的番茄酱"
            font.pixelSize: 32
            font.weight: Font.Bold
        }

        Label {
            id: motto
            anchors.horizontalCenter: parent.horizontalCenter
            text: "我以前没得选择，现在我想做一个好人。——阿明"
            font.pixelSize: 16
            font.weight: Font.Light
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 32
            Repeater {

                model: ListModel {
                    id: listModel
                }

                delegate: Rectangle {
                    id: item2
                    width: 64
                    height: 32
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: model.text
                        font.pixelSize: 16
                    }

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            Qt.openUrlExternally(model.url)
                        }

                        onEntered: {
                            item2.color = "#eee"
                        }
                        onExited: {
                            item2.color = "transparent"
                        }
                    }
                }
            }
        }
        GridLayout {
            visible: $Controller.isMobile()
            columns: 4
            SideMenu {
                id: musicPlayer
                iconSource: "qrc:/icon/music_64x64.png"
                onClicked: {
                    pushPageToStack("MusicPlayer.qml")
                }
            }

            SideMenu {
                id: webBrowser
                iconSource: "qrc:/icon/browser_64x64.png"
                onClicked: {
                    pushPageToStack("WebBrowser.qml")
                }
            }
            SideMenu {
                iconSource: "qrc:/icon/notebook_128x128.png"
                onClicked: {
                    pushPageToStack("NoteListView.qml")
                }
            }
            SideMenu {
                iconSource: "qrc:/icon/settings_64x64.png"
                onClicked: {
                    pushPageToStack("SettingView.qml")
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Label {
                text: "Powered by "
                color: "#aaa"
            }
            Label {
                text: "Qt " + $Controller.qtVersion()
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally("https://doc.qt.io/")
                    }
                }
            }
        }


    }
    Component.onCompleted: {
        listModel.append({"text": "首页", "url": "https://blog.pikachu.net.cn/"})
        listModel.append({"text": "归档", "url": "https://blog.pikachu.net.cn/archives/"})
        listModel.append({"text": "标签", "url": "https://blog.pikachu.net.cn/tags/"})
        listModel.append({"text": "关于", "url": "https://blog.pikachu.net.cn/post/about/"})
    }
}
