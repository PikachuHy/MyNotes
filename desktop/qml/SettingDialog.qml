import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import Controller 1.0
Dialog {
//    Layout.fillWidth: true

    default property alias content: _mainLayout.data
//    property alias spacing: _mainLayout.spacing

//    color: FishUI.Theme.darkMode ? "#363636" : "#FFFFFF"
//    radius: FishUI.Theme.mediumRadius
    implicitWidth: 500
    implicitHeight: 400
    title: "Setting"
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    property int largeSpacing: 12
    ColumnLayout {
        id: _mainLayout
        anchors.fill: parent
        anchors.leftMargin: largeSpacing * 1.5
        anchors.rightMargin: largeSpacing * 1.5
        anchors.topMargin: largeSpacing
        anchors.bottomMargin: largeSpacing

        GridLayout {
            columns: 2
            columnSpacing: largeSpacing * 1.5
            rowSpacing: largeSpacing

            Label {
                text: qsTr("Render")
            }

            ComboBox {
                id: renderComboBox
                Layout.fillWidth: true
                model: ["Web Engine", "Text Browser", "Qt Widget", "Qt Quick"]

                leftPadding: largeSpacing
                rightPadding: largeSpacing
                topInset: 0
                bottomInset: 0
                onActivated: {

                }
            }
        }
    }
    SettingDialogController {
        id: controller
    }

    onAccepted: {
        console.log('render:', renderComboBox.currentIndex)
        controller.render = renderComboBox.currentIndex
    }

    onRejected: {

    }
    Component.onCompleted: {
        renderComboBox.currentIndex = controller.render
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

