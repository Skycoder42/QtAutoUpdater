import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

DialogBase {
	id: progressDialog

	title: qsTr("Checking for updates…")

	visible: updater ? updater.state == Updater.Checking : false
	onVisibleChanged: btnBox.standardButton(DialogButtonBox.Cancel).enabled = true  // reset button box when dialog is hidden or shown

	ProgressItem {
		id: progressItem
		anchors.fill: parent

		Connections {
			target: updater

			onProgressChanged: {
				progressItem.progress = progress;
				progressItem.status = status;
			}
		}
	}

	footer: Item {
		implicitWidth: btnBox.implicitWidth
		implicitHeight: btnBox.implicitHeight

		DialogButtonBox {
			id: btnBox
			anchors.fill: parent

			standardButtons: DialogButtonBox.Cancel

			onClicked: {
				if (button === standardButton(DialogButtonBox.Cancel)) {
					button.enabled = false;
					updater.abortUpdateCheck();
				}
			}
		}
	}
}
