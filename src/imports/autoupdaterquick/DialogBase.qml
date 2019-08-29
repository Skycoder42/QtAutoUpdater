import QtQuick 2.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

Dialog {
	id: dialogBase

	property Updater updater: null

	modal: true
	closePolicy: Dialog.CloseOnEscape
	anchors.centerIn: Overlay.overlay
	margins: 16
}
