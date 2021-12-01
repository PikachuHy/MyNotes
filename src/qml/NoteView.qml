import QtQuick 2.15
import QtQuick.Controls 2.15
import cn.net.pikachu.control 1.0
import QtMarkdown 1.0

Flickable {
    property string source
    property var pathList: []
    id: mdPage
    contentHeight: md.implicitHeight
    contentWidth: md.width
    onWidthChanged: {
        console.log('width change')
    }

    QtQuickMarkdownEditor {
        id: md
        visible: true
        width: parent.width
        onHeightChanged: {

            // console.log('height change', md.height, md.implicitHeight)
            // mdPage.contentHeight = md.implicitHeight
        }
        onShowInputMethod: {
            // Qt.inputMethod.show()
        }
        onImageClicked: function (path) {
            console.log('click image:', path)
            Qt.inputMethod.hide()
            if (path.endsWith(".gif")) {
                previewGif.source = 'file://' + path
                previewGifPopup.visible = true
            } else {
                previewImage.source = 'file://' + path
                previewImagePopup.visible = true
            }

        }
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
    Popup {
        id: previewGifPopup
        visible: false
        anchors.centerIn: parent
        ScrollView {
            anchors.fill: parent

            AnimatedImage {
                id: previewGif
            }
        }
    }
    Component.onCompleted: {
        for(var i=0;i<pathList.length;i++) {
            md.addPath(pathList[i])
        }


        md.source = source
    }
}
