import QtQuick
import cn.net.pikachu.control

Flickable {
    property alias source: md.source
    property alias path: md.path
    id: mdPage
    anchors.fill: parent
    contentHeight: md.implicitHeight
    contentWidth: parent.width
    onWidthChanged: {
        console.log('width change')
    }

    QtQuickMarkdownItem {
        id: md
        visible: true
        anchors.fill: parent
        onHeightChanged: {

            // console.log('height change', md.height, md.implicitHeight)
            // mdPage.contentHeight = md.implicitHeight
        }
    }
    Component.onCompleted: {

    }
}
