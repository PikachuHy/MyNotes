import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import cn.net.pikachu.control 1.0
import Controller 1.0

Window {
    id: root
    width: 800
    height: 600
    title: "MyNotes"
    visible: true

    Rectangle {
        anchors.fill: parent

        NoteTreeView {
            width: 300
            height: parent.height
            onNoteClicked: {
                editor.path = path
                editor.source = path
                controller.setLastOpenedNote(path)
            }
        }
        ScrollView {

            x: 305
            width: parent.width - 308
            height: parent.height

            QtQuickMarkdownItem {
                id: editor
                width: parent.width
                onCodeCopied: function (code) {
                    console.log('copy code:', code)
                    clipboard.copyText(code)
                    root.showPassiveNotification('code copied', 800)
                }
                onLinkClicked: function (url) {
                    console.log('click link:', url)
                    root.showPassiveNotification('open url: ' + url, 'short',
                                                 'open', function () {
                                                     console.log('open', url)
                                                     controller.openUrl(url)
                                                 })
                }
                onImageClicked: function (path) {
                    console.log('click image:', path)
                    previewImage.source = 'file://' + path
                    previewImagePopup.visible = true
                }
            }
        }
        Controller {
            id: controller
        }
        SettingDialog {
            id: settingDialog
            visible: false
        }
        Keys.onPressed: {
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

    function showPassiveNotification(message, timeout, actionText, callBack) {
        if (!internal.passiveNotification) {
            var component = Qt.createComponent("qrc:/fishui/kit/Toast.qml")
            internal.passiveNotification = component.createObject(root)
        }

        internal.passiveNotification.showNotification(message, timeout,
                                                      actionText, callBack)
    }
    Component.onCompleted: {
        editor.height = root.height
        editor.path = controller.lastOpenedNote()
        editor.source = controller.lastOpenedNote()
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
}
