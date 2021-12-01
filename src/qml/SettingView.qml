import QtQuick 2.15
import QtQuick.Controls 2.15
import Controller 1.0

Rectangle {
    id: settingView
    width: 400
    height: 800
    color: "#eee"
    property int largeSpacing: 12
    component SettingItem: Rectangle {
        property alias key: keyLabel.text
        property alias value: valueLabel.text
        width: parent.width
        height: 64
        radius: 10
        Column {
            width: parent.width
            spacing: 5
            Label {
                id: keyLabel
                font.pixelSize: 24
            }
            Label {
                id: valueLabel
                width: parent.width
                wrapMode: Text.WordWrap
            }
        }
    }
    Column {
        width: parent.width
        spacing: 10
        SettingItem {
            key: qsTr("Data Path")
            value: $Controller.noteDataPath()
        }

        SettingItem {
            key: qsTr("Config Path")
            value: $Controller.configStorePath()
        }
        Rectangle {
            width: parent.width
            height: 64
            radius: 10
            Column {
                width: parent.width
                spacing: 5

                Label {
                    font.pixelSize: 24
                    text: qsTr("Render")
                }

                ComboBox {
                    id: renderComboBox
                    width: parent.width
                    model: ["Web Engine", "Text Browser", "Qt Widget", "Qt Quick Desktop", "Qt Quick Mobile"]

                    leftPadding: largeSpacing
                    rightPadding: largeSpacing
                    topInset: 0
                    bottomInset: 0
                    onActivated: {
                        controller.render = renderComboBox.currentIndex
                    }
                }
            }
        }
        Button {
            text: "Change Data Path"
            onClicked: {
                settingView.visible = false
                var FileListViewPage = Qt.createComponent(
                            "FileListView.qml").createObject(listContainer, {
                                                                 "x": 0,
                                                                 "y": 0,
                                                                 "width": listContainer.width,
                                                                 "height": listContainer.height,
                                                                 "path": $FileSystem.defaultPath()
                                                             })
                pageStack.push(FileListViewPage)
            }
        }
    }
    SettingDialogController {
        id: controller
    }
    Component.onCompleted: {
        renderComboBox.currentIndex = controller.render
    }
}
