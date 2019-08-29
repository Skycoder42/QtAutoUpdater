import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

DialogBase {
	id: resultDialog
	visible: false

	property bool showOnSuccess: false
	property bool showOnFailure: true
	property bool showOnNoUpdates: true
	property bool autoRunUpdater: false
	property bool forceOnExit: false

	title: qsTr("Check for updates finished!")
	standardButtons: Dialog.Ok

	Connections {
		target: updater

		onCheckUpdatesDone: {
			switch (result) {
			case Updater.NewUpdates:
				// trigger automatic update installation if requested
				let showInfo = showOnSuccess;
				if (autoRunUpdater) {
					updater.runUpdater(forceOnExit);
					showInfo = updater.runOnExit;
				}

				// show update success info
				if (showInfo) {
					contentLabel.text = qsTr("New updates are available.");
					if (updater.runOnExit)
						contentLabel.text += qsTr(" The update tool will be started to install those as soon as you close the application.");
					resultDialog.open();
				}
				break;
			case Updater.NoUpdates:
				if (showOnNoUpdates) {
					contentLabel.text = qsTr("No new updates available.");
					resultDialog.open();
				}
				break;
			case Updater.Error:
				if (showOnFailure) {
					contentLabel.text = qsTr("An error occured while trying to check for updates.");
					resultDialog.open();
				}
				break;
			default:
				break;
			}
		}
	}

	Label {
		id: contentLabel
		anchors.fill: parent

		wrapMode: Label.Wrap
	}
}
