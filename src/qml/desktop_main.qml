import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import cn.net.pikachu.control 1.0
import Controller 1.0
import QtMarkdown 1.0

Window {
    id: root
    x: controller.lastWindowRect().x
    y: controller.lastWindowRect().y
    width: controller.lastWindowRect().width
    height: controller.lastWindowRect().height
    title: "MyNotes"
    visible: true

    Row {
        Column {
            id: debug1
            width: 64
            SideMenu {
                iconSource: "qrc:/icon/avatar_300x300.png"
                onClicked: {
                    stackLayout.currentIndex = 0
                }
            }

            SideMenu {
                iconSource: "qrc:/icon/notebook_128x128.png"
                onClicked: {
                    stackLayout.currentIndex = 1
                }
            }

            SideMenu {
                iconSource: "qrc:/icon/music_64x64.png"
                onClicked: {
                    stackLayout.currentIndex = 2
                }
            }

            SideMenu {
                iconSource: "qrc:/icon/browser_64x64.png"
                onClicked: {
                    stackLayout.currentIndex = 3
                }
            }

            SideMenu {
                iconSource: "qrc:/icon/settings_64x64.png"
                onClicked: {
                    settingDialog.visible = true
                }
            }
        }
        StackLayout {
            id: stackLayout
            width: root.width - 64
            Profile {}

            Row {
                id: notebook
                NoteTreeView {
                    width: 300
                    height: root.height
                    onNoteClicked: function (path) {
                        loadNote(path)
                    }
                }

                Flickable {
                    id: editorContainer
                    width: notebook.width - 300
                    height: root.height
                    QtQuickMarkdownEditor {
                        id: editor
                        focus: true
                        width: parent.width

                        onCodeCopied: function (code) {
                            console.log('copy code:', code)
                            clipboard.copyText(code)
                            root.showPassiveNotification('code copied', 800)
                        }
                        onLinkClicked: function (url) {
                            console.log('click link:', url)
                            root.showPassiveNotification('open url: ' + url,
                                                         'short', 'open',
                                                         function () {
                                                             console.log('open',
                                                                         url)
                                                             controller.openUrl(
                                                                         url)
                                                         })
                        }
                        onImageClicked: function (path) {
                            console.log('click image:', path)
                            previewImage.source = 'file://' + path
                            previewImagePopup.visible = true
                        }
                        onHeightChanged: {
                            // console.log('onImplicitHeightChanged', editor.implicitHeight)
                            editorContainer.contentHeight = editor.height
                        }
                        onDocSave: {
                            showPassiveNotification("Save Note Success")
                        }
                    }
                }
            }

            MusicPlayer {}

            WebBrowser {}

            Pane {}
        }

        Controller {
            id: controller
            onNoteChanged: function (path) {
                if (path === editor.path) {
                    loadNote(path)
                }
            }
        }
        SettingDialog {
            id: settingDialog
            visible: false
        }
        Keys.onPressed: event => {
                            if (event.key === Qt.Key_Comma
                                && event.modifiers === Qt.ControlModifier) {
                                console.log('show settings dialong')
                                settingDialog.visible = true
                                event.accepted = true
                            }
                        }
    }

    QtObject {
        id: internal
        property QtObject passiveNotification
    }
    onXChanged: {
        saveNewWindowGeometry()
    }
    onYChanged: {
        saveNewWindowGeometry()
    }
    onWidthChanged: {
        saveNewWindowGeometry()
    }
    onHeightChanged: {
        saveNewWindowGeometry()
    }
    function saveNewWindowGeometry() {
        var rect = Qt.rect(root.x, root.y, root.width, root.height)
        console.log(rect)
        controller.setLastWindowRect(rect)
    }

    function showPassiveNotification(message, timeout, actionText, callBack) {
        if (!internal.passiveNotification) {
            var component = Qt.createComponent("qrc:/Toast.qml")
            internal.passiveNotification = component.createObject(root)
        }

        internal.passiveNotification.showNotification(message, timeout,
                                                      actionText, callBack)
    }
    function loadNote(path) {
        console.log('load path', path)
        editor.width = editorContainer.width
        editor.addPath($FileSystem.fileDir(path))
        editor.source = path
        controller.setLastOpenedNote(path)
        controller.watchNote(path)
    }

    Component.onCompleted: {
        editor.height = root.height
        var path = controller.lastOpenedNote()
        loadNote(path)
        console.log('stackLayout z: ', stackLayout.z)
    }
    Popup {
        id: previewImagePopup
        visible: false
        anchors.centerIn: parent
        ScrollView {
            anchors.fill: parent

            Image {
                id: previewImage
            }
        }
    }
    Dialog {
        id: newNoteDialg
        title: "New Note"
        anchors.centerIn: parent
        standardButtons: Dialog.Ok | Dialog.Cancel
        TextEdit {}
    }
}
