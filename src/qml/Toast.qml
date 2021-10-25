import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
// import QtGraphicalEffects 1.0

Popup {
    id: control
    property int largeSpacing: 12
    property int smallSpacing: 6
    x: Math.round(parent.width / 2 - width / 2)
    y: parent.height - height - largeSpacing
    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentWidth + leftPadding + rightPadding) + leftInset + rightInset
    implicitHeight: Math.max(background ? background.implicitHeight : 0 ,
                             contentHeight + topPadding + bottomPadding)+ topInset + bottomInset
    height: implicitHeight
    width: implicitWidth

    topPadding: smallSpacing
    leftPadding: smallSpacing
    rightPadding: smallSpacing
    bottomPadding: largeSpacing

    modal: false
    closePolicy: Popup.NoAutoClose
    focus: false
    clip: false

    function showNotification(message, timeout, actionText, callBack) {
        if (!message) {
            return
        }

        let interval = 7000

        if (timeout === "short") {
            interval = 4000
        } else if (timeout === "long") {
            interval = 12000
        } else if (timeout > 0) {
            interval = timeout
        }

        open()

        for (var i = 0; i < outerLayout.children.length - 3; ++i) {
            outerLayout.children[i].close()
        }

        let delegate = delegateComponent.createObject(outerLayout, {
            "text": message,
            "actionText": actionText || "",
            "callBack": callBack || (function(){}),
            "interval": interval
        });

        // Reorder items to have the last on top
        let children = outerLayout.children;
        for (i in children) {
            children[i].Layout.row = children.length - 1 - i
        }
    }

    background: Item {}

    contentItem: GridLayout {
        id: outerLayout
        columns: 1
    }

    Component {
        id: delegateComponent

        Control {
            id: delegate
            property alias text: label.text
            property alias actionText: actionButton.text
            property alias interval: timer.interval
            property var callBack
            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: -delegate.height
            opacity: 0

            function close() {
                closeAnim.running = true;
            }

            leftPadding: largeSpacing
            rightPadding: largeSpacing
            topPadding: largeSpacing
            bottomPadding: largeSpacing

            Component.onCompleted: openAnim.restart()
            ParallelAnimation {
                id: openAnim
                OpacityAnimator {
                    target: delegate
                    from: 0
                    to: 1
                    duration: 400
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: delegate
                    property: "Layout.bottomMargin"
                    from: -delegate.height
                    to: 0
                    duration: 400
                    easing.type: Easing.InOutQuad
                }
            }

            SequentialAnimation {
                id: closeAnim
                ParallelAnimation {
                    OpacityAnimator {
                        target: delegate
                        from: 1
                        to: 0
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: delegate
                        property: "Layout.bottomMargin"
                        to: -delegate.height
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }
                ScriptAction {
                    script: delegate.destroy();
                }
            }

            contentItem: RowLayout {
                id: mainLayout

                width: mainLayout.width
                //FIXME: why this is not automatic?
                implicitHeight: Math.max(label.implicitHeight, actionButton.implicitHeight)

                HoverHandler {
                    id: hover
                }
                TapHandler {
                    acceptedButtons: Qt.LeftButton
                    onTapped: delegate.close();
                }
                Timer {
                    id: timer
                    running: control.visible && !hover.hovered
                    onTriggered: delegate.close();
                }

                Label {
                    id: label
                    Layout.maximumWidth: Math.min(control.parent.width - largeSpacing * 4, implicitWidth)
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 4
                }

                Button {
                    id: actionButton
                    visible: text.length > 0
                    onClicked: {
                        delegate.close();;
                        if (delegate.callBack && (typeof delegate.callBack === "function")) {
                            delegate.callBack();
                        }
                    }
                }
            }

            background: Rectangle {
                color: "#F3F4F9"
                radius: 10.0
                opacity: 0.9
                layer.enabled: true
                /*
                layer.effect: DropShadow {
                    transparentBorder: true
                    radius: 32
                    samples: 32
                    horizontalOffset: 0
                    verticalOffset: 0
                    color: Qt.rgba(0, 0, 0, 0.14)
                }
                */
            }
        }
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.4)
    }

    Overlay.modeless: Item {}
}
