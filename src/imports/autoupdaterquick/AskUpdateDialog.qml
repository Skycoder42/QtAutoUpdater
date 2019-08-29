import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

DialogBase {
	id: askDialog
	visible: false

	title: qsTr("Check for updates?")
	standardButtons: Dialog.Yes | Dialog.No

	onAccepted: updater.checkForUpdates()

	Label {
		id: contentLabel
		anchors.fill: parent

		wrapMode: Label.Wrap

		text: qsTr("Do you want to check for updates now?")
	}
}
