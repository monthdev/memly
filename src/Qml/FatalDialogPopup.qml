// FatalDialogPopup.qml
import QtQuick
import QtQuick.Dialogs

MessageDialog {
    id: fatalDialog
    title: "Fatal Error"
    text: FatalErrorBridge.message
    buttons: MessageDialog.Ok
    modality: Qt.ApplicationModal

    onAccepted: FatalErrorBridge.AcknowledgeAndExit()

    Connections {
        target: FatalErrorBridge
        function onShowFatalDialog() { fatalDialog.open() }
    }
}
