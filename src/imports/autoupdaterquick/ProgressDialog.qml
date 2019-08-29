import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

DialogBase {
	id: progressDialog

	property real progress: -1.0
	property alias status: statusLabel.text

	readonly property real indeterminate: progressDialog.progress < 0

	title: qsTr("Checking for updates…")

	visible: updater ? updater.state == Updater.Checking : false
	onVisibleChanged: btnBox.standardButton(DialogButtonBox.Cancel).enabled = true  // reset button box when dialog is hidden or shown

	Connections {
		target: updater

		onProgressChanged: {
			progressDialog.progress = progress;
			progressDialog.status = status;
		}
	}

	GridLayout {
		anchors.fill: parent
		columns: 2

		ProgressBar {
			id: progressBar
			Layout.fillWidth: true
			Layout.columnSpan: progressDialog.indeterminate ? 2 : 1

			from: 0.0
			to: 1.0
			value: progressDialog.progress
			indeterminate: progressDialog.indeterminate
		}

		Label {
			id: percentLabel
			visible: !progressDialog.indeterminate
			text: qsTr("%1%").arg((progressDialog.progress * 100).toFixed(0))
		}

		Label {
			id: statusLabel
			Layout.columnSpan: 2
			Layout.fillWidth: true
			Layout.fillHeight: true

			font.italic: true
			elide: Text.ElideMiddle
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
