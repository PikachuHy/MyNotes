import QtQuick
import QtWebView
import QtQuick.Layouts

Rectangle {
    property alias url: webView.url
    ColumnLayout {
        width: parent.width
        height: parent.height
        Row {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            spacing: 0
            Rectangle {
                width: parent.width - 32
                height: parent.height
                color: '#eee'
                radius: 10
                TextInput {
                    id: urlText
                    x: 10
                    width: parent.width - 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: ""
                    autoScroll: false
                    Keys.onReturnPressed: {
                        if (text.startsWith("http://") || text.startsWith(
                                    "https://")) {
                            webView.url = text
                        } else {
                            var url
                            if ($Controller.isDesktop()) {
                                url = "http://www.baidu.com/s?wd=" + text
                            } else {
                                url = "http://www.baidu.com/s?word=" + text
                            }
                            webView.url = url
                        }

                        Qt.inputMethod.hide()
                    }
                }
            }

            Image {
                width: 32
                height: 32
                source: "qrc:/icon/reload_64x64.png"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        webView.reload()
                    }
                }
            }
        }

        WebView {
            id: webView
            url: "http://www.baidu.com"
            Layout.fillHeight: true
            Layout.fillWidth: true
            onUrlChanged: {
                urlText.text = url
            }
        }
    }
}
