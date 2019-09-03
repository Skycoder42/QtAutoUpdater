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
	property int textMode: UpdateButton.Dynamic
	property bool allowInstall: true
	property bool runOnExit: false

	property real _currentProgress: -1.0
	property string _currentStatus

	enabled: updater
	delay: 0
	checkable: false
	leftPadding: (indeterminateIndicator.visible ?
					  indeterminateIndicator.width + rightPadding/4 :
					  0)
				 + rightPadding
	text: {
		switch (updater ? updater.state : Updater.Error) {
		case Updater.NoUpdates:
		case Updater.Error:
			return qsTr("Check for updates");
		case Updater.Checking:
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
			break;
		case Updater.NewUpdates:
			return allowInstall ?
						qsTr("Install updates") :
						qsTr("Found new updates");
		case Updater.Installing:
			return qsTr("Installing updates…");
		}
	}
	progress: {
		switch (updater ? updater.state : Updater.Error) {
		case Updater.NoUpdates:
		case Updater.Error:
			return 0.0;
		case Updater.Checking:
			return Math.max(_currentProgress, 0.0);
		case Updater.NewUpdates:
		case Updater.Installing:
			return 1.0;
		}
	}

	BusyIndicator {
		id: indeterminateIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.topMargin: updateButton.topPadding
		anchors.bottomMargin: updateButton.bottomPadding
		anchors.leftMargin: updateButton.rightPadding * 0.75
		width: height
		visible: updater && updater.running && _currentProgress < 0
		running: true
		padding: 0
	}

	MouseArea {
		id: blockArea
		anchors.fill: parent
		visible: updater &&
				 (updater.state == Updater.Installing ||
				  (updater.state == Updater.Checking && textMode != UpdateButton.Cancel) ||
				  (updater.state == Updater.NewUpdates && !allowInstall))
		acceptedButtons: Qt.AllButtons
		z: 100
	}

	Connections {
		target: updater
		onProgressChanged: {
			_currentProgress = progress;
			if (status != "")
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

	FontMetrics {
		id: fillerMetrics
		font: updateButton.font

		function calcSpacer(width) {
			let baseWidth = fillerMetrics.boundingRect(" ").width;
			console.log(baseWidth, width, Math.ceil(width / baseWidth));
			return "&nbsp;".repeat(Math.ceil(width / baseWidth));
		}
	}
}
