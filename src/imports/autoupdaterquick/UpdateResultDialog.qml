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
	property int autoInstallMode: Updater.Parallel
	property int installScope: Updater.PreferInternal

	title: qsTr("Check for updates finished!")
	standardButtons: Dialog.Ok
	onStandardButtonsChanged: {
		if (contentLabel.state == Updater.NewUpdates) {
			resultDialog.standardButton(Dialog.Ok).text = qsTr("Install now");
			resultDialog.standardButton(Dialog.Apply).text = qsTr("Install on exit");
			resultDialog.standardButton(Dialog.Cancel).text = qsTr("Install later");
		} else
			resultDialog.standardButton(Dialog.Ok).text = qsTr("OK");
	}

	onAccepted: {
		if (contentLabel.state == Updater.NewUpdates)
			updater.runUpdater(Updater.Parallel, installScope);
	}
	onApplied: {
		close();
		updater.runUpdater(Updater.ForceOnExit, installScope);
	}

	Connections {
		target: updater

		onCheckUpdatesDone: {
			switch (result) {
			case Updater.NewUpdates:
				// trigger automatic update installation if requested
				let showInfo = showOnSuccess;
				if (autoRunUpdater) {
					updater.runUpdater(autoInstallMode, installScope);
					showInfo = updater.runOnExit;
				}

				// show update success info
				if (showInfo) {
					contentLabel.state = updater.runOnExit ? (result + "-auto-exit") : result
					resultDialog.open();
				}
				break;
			case Updater.NoUpdates:
				if (showOnNoUpdates) {
					contentLabel.state = result
					resultDialog.open();
				}
				break;
			case Updater.Error:
				if (showOnFailure) {
					contentLabel.state = result
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

		states: [
			State {
				name: Updater.NoUpdates
				PropertyChanges {
					target: contentLabel
					text: qsTr("No new updates available.");
				}
			},
			State {
				name: Updater.NewUpdates
				PropertyChanges {
					target: contentLabel
					text: qsTr("New updates are available! You can install them now or later");
				}
				PropertyChanges {
					target: resultDialog
					standardButtons: Dialog.Ok | Dialog.Apply | Dialog.Cancel
				}
			},
			State {
				name: Updater.NewUpdates + "-auto-exit"
				PropertyChanges {
					target: contentLabel
					text: qsTr("New updates are available and will be installed as soon as you close the application.");
				}
			},
			State {
				name: Updater.Error
				PropertyChanges {
					target: contentLabel
					text: qsTr("An error occured while trying to check for updates.");
				}
			}

		]
	}
}
