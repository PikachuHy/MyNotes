import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dialog
    property alias placeholderText: textField.placeholderText
    property alias text: textField.text
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent
    focus: true
    TextField {
        id: textField
        focus: true
        anchors.fill: parent
        // TODO: 按回车，等同于点击OK
    }


    function show() {
        textField.text = ""
        dialog.visible = true
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
