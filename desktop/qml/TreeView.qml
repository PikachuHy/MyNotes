import QtQuick 2.15
import QtQml.Models 2.15
import cn.net.pikachu.control 1.0

Rectangle {
    id: control
    required property var model2
    // property Component delegate
    property var currentIndex
    signal itemClicked(var index)
    signal itemRightClicked(var index)

    ListView {
        id: listView
        width: parent.width
        height: parent.height

        highlight: Rectangle {
            width: listView.width
            height: 32
            color: "#eee"
            // color: "red"
            y: listView.currentItem.y
            // Behavior on y { SpringAnimation { spring: 2; damping: 0.1 } }
        }
        highlightFollowsCurrentItem: false

        model: TreeModelAdaptor {
            id: my_model
            model: control.model2
        }

        delegate: Item {
            width: listView.width
            height: 32
            Rectangle {
                id: item
                height: 32
                width: listView.width
                color: "transparent"
            }

            Row {
                spacing: 10
                leftPadding: 32 * model._q_TreeView_ItemDepth
                Image {
                    width: 32
                    height: 32
                    source: "qrc://" + model.iconPath
                }
                Text {
                    width: listView.width - 32 * model._q_TreeView_ItemDepth - 32 - 10
                    text: model.text
                    anchors.verticalCenter: parent.verticalCenter
                    elide: Text.ElideMiddle
                }
            }

            MouseArea {
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                anchors.fill: parent
                onClicked: (mouse) => {
                    // console.log('click', index)
                    console.log(model.text)
                    var modelIndex = model._q_TreeView_ModelIndex
                    control.currentIndex = modelIndex
                    control.itemClicked(modelIndex)
                    listView.currentIndex = index
                    if (mouse.button === Qt.RightButton) {
                        control.itemRightClicked(modelIndex)
                    }
                }
                onDoubleClicked: {
                    // console.log('double click', index)
                    console.log(model.text)
                    if (my_model.isExpanded(index)) {
                        // console.log('collapse')
                        my_model.collapseRow(index)
                    } else {
                        // console.log('expand')
                        my_model.expandRow(index)
                    }
                }

                onEntered: {
                    item.color = "#eee"
                }
                onExited: {
                    item.color = "transparent"
                }
            }
            Keys.onPressed: event => {
                                console.log('press E')
                                var index = itemSelectionModel.currentIndex
                                if (controller.isNote(index)) {
                                    var path = controller.getNoteFullPath(index)
                                    console.log('path', path)
                                    controller.openInTypora(path)
                                }
                            }
        }
    }
    function setCurrentIndex(index) {

    }
}
