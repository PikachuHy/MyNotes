import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.12
import QtQml.Models 2.15
import cn.net.pikachu.control 1.0
import QtQuick.Layouts 1.15

Rectangle {
    id: control
    required property var model
    property Component delegate
    property var currentIndex
    property var currentModelIndex
    signal itemClicked(var index)
    signal itemRightClicked(var index)
    // 需要它水平滑动
    Flickable {
        id: flickContainer
        width: parent.width
        height: parent.height
        contentHeight: parent.height
        contentWidth: parent.width
        clip: true

        ListView {
            id: listView
            width: parent.width
            height: parent.height
            contentWidth: 600

            model: TreeModelAdaptor {
                id: my_model
                model: control.model
            }

            delegate: Item {
                id: listDelegate
                width: listView.width
                height: 32
                property alias color: item.color
                property alias bgWidth: displayControl.width
                property int indentWidth: model._q_TreeView_ItemDepth * 32
                property alias realItemWidth: displayControl.contentItem.width
                property alias contentItem: displayControl.contentItem
                property bool selected: false
                RowLayout {
                    id: rowLayout
                    spacing: 0
                    Item {
                        id: intent
                        width: model._q_TreeView_ItemDepth * 32
                    }

                    Control {
                        id: displayControl
                        Layout.fillWidth: true
                        contentItem: control.delegate.createObject(
                                         displayControl, {
                                             "model": model
                                         })
                        background: Rectangle {
                            id: item
                            width: displayControl.width
                                   > listView.width ? displayControl.width : listView.width
                            color: "transparent"
                        }
                    }
                }
                MouseArea {
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    anchors.fill: parent
                    onClicked: mouse => {
                                   console.log(model.text)
                                   var modelIndex = model._q_TreeView_ModelIndex
                                   control.currentModelIndex = modelIndex
                                   listDelegate.selected = true
                                   control.itemClicked(modelIndex)
                                   listView.itemAtIndex(
                                       listView.currentIndex).color = "transparent"
                                   listView.itemAtIndex(
                                       listView.currentIndex).selected = false
                                   listView.currentIndex = index
                                   item.color = "#eee"
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
                        // 同步修改的话，数据有问题，
                        timer.start()
                    }

                    onEntered: {
                        item.color = "#eee"
                    }
                    onExited: {
                        if (!listDelegate.selected)
                            item.color = "transparent"
                    }
                }
                Keys.onPressed: event => {
                                    console.log('press E')
                                    var index = itemSelectionModel.currentIndex
                                    if (controller.isNote(index)) {
                                        var path = controller.getNoteFullPath(
                                            index)
                                        console.log('path', path)
                                        controller.openInTypora(path)
                                    }
                                }
            }
        }
    }
    Timer {
        id: timer
        interval: 100 // 这个值如果太小，可能没法拿全item的数据
        repeat: false
        onTriggered: {
            // 比较hack的做法，让listView折叠时改变宽度
            // 计算新的宽度
            // 这里拿到的数据不准
            let maxWidth = 300 // 这个值怎么确定呢？
            // console.log('control width', listView.width)
            console.log('count', listView.count)
            for (var i = 0; i < listView.count; i++) {
                let it = listView.itemAtIndex(i)
                if (!it)
                    continue
                // console.log(it.contentItem.text ,it.realItemWidth, it.indentWidth, it.realItemWidth + it.indentWidth, maxWidth)
                if (it.realItemWidth + it.indentWidth > maxWidth) {
                    maxWidth = it.realItemWidth + it.indentWidth
                }
            }
            flickContainer.contentWidth = maxWidth
            listView.update()
        }
    }
}
