import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

DelayButton {
	id: updateButton

	enum TextMode {
		Dynamic,
		Static,
		Cancel
	}

	property Updater updater: null
	property int textMode: UpdateButton.Cancel
	property bool allowInstall: true
	property bool runOnExit: false

	property real _currentProgress: -1.0
	property string _currentStatus

	states: [
		State {
			name: Updater.NoUpdates
		},
		State {
			name: Updater.Checking
			PropertyChanges {
				target: updateButton
				text: {
					switch (textMode) {
					case UpdateButton.Dynamic:
						return _currentStatus == "" ?
									qsTr("Checking for updates…") :
									_currentStatus;
					case UpdateButton.Static:
						return qsTr("Checking for updates…");
					case UpdateButton.Cancel:
						return qsTr("Cancel");
					}
				}
				progress: Math.max(_currentProgress, 0.0)
			}
			PropertyChanges {
				target: blockArea
				visible: textMode != UpdateButton.Cancel
			}
			PropertyChanges {
				target: indeterminateIndicator
				visible: _currentProgress < 0
			}
		},
		State {
			name: Updater.Canceling
			PropertyChanges {
				target: updateButton
				text: qsTr("Canceling…")
				progress: Math.max(_currentProgress, 0.0)
			}
			PropertyChanges {
				target: blockArea
				visible: true
			}
			PropertyChanges {
				target: indeterminateIndicator
				visible: _currentProgress < 0
			}
		},
		State {
			name: Updater.NewUpdates
			PropertyChanges {
				target: updateButton
				text: allowInstall ?
						  qsTr("Install updates") :
						  qsTr("Found new updates")
				progress: 1.0
			}
			PropertyChanges {
				target: blockArea
				visible: !allowInstall
			}
		},
		State {
			name: Updater.Installing
			PropertyChanges {
				target: updateButton
				text: qsTr("Installing updates…")
				progress: 1.0
			}
			PropertyChanges {
				target: blockArea
				visible: true
			}
			PropertyChanges {
				target: indeterminateIndicator
				visible: true
			}
		},
		State {
			name: Updater.Error
		}
	]

	state: updater ? updater.state : Updater.Error

	enabled: updater
	delay: 0
	checkable: false
	leftPadding: (indeterminateIndicator.visible ?
					  indeterminateIndicator.width + rightPadding/4 :
					  0)
				 + rightPadding
	text: qsTr("Check for updates")
	progress: 0.0

	BusyIndicator {
		id: indeterminateIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.topMargin: updateButton.topPadding
		anchors.bottomMargin: updateButton.bottomPadding
		anchors.leftMargin: updateButton.rightPadding * 0.75
		width: height
		visible: false
		running: true
		padding: 0
	}

	MouseArea {
		id: blockArea
		anchors.fill: parent
		visible: !updater
		acceptedButtons: Qt.AllButtons
		z: 100
	}

	Connections {
		target: updater
		onProgressChanged: {
			_currentProgress = progress;
			_currentStatus = status;
		}

		onCheckUpdatesDone: {
			_currentProgress = -1.0;
			_currentStatus = "";
		}
	}

	onClicked: {
		switch (updater ? updater.state : Updater.Error) {
		case Updater.NoUpdates:
		case Updater.Error:
			updater.checkForUpdates();
			break;
		case Updater.NewUpdates:
			if (allowInstall)
				updater.runUpdater(runOnExit);
			break;
		case Updater.Checking:
			if (textMode == UpdateButton.Cancel)
				updater.abortUpdateCheck();
		default:
			break;
		}
	}
}
