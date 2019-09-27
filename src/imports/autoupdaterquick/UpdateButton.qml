import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief A simple button for update checks
 *
 * @extends QtQuick.Controls.DelayButton
 *
 * The UpdateButton is a ready-made "button" to check for updates. It is connected to an updater and
 * thus will automatically start checking for updates. It will also display a progress and
 * optionally allow canceling and installation of updates.
 *
 * The button will represent the updaters state and automatically enable/disable itself to
 * avoid start beeing called twice. In addition to that, the whole button item will be disabled
 * if the updater gets deleted while the button is still existant.
 *
 * @sa QtAutoUpdater::Updater, QtAutoUpdater::Updater::checkForUpdates,
 * @ref qtautoupdater_image_page "Image Page"
 */
DelayButton {
	id: updateButton

	//! The different modes of what text to show on the button while checking for updates
	enum TextMode {
		Dynamic,  //!< If available the check status is shown as text. Otherwise same as Static
		Static,  //!< A static "checking for updates" text is shown
		Cancel  //!< The text shows "Cancel", and when clicked, the button will cancel the update check
	}

	/*! @brief The updater to be used by the button
	 *
	 * @default{`null`}
	 *
	 * @accessors{
	 *	@memberAc{updater}
	 * }
	 * @sa QtAutoUpdater::Updater
	 */
	property Updater updater: null
	/*! @brief The text mode while checking for updates
	 *
	 * @default{`UpdateButton::Cancel`}
	 *
	 * This mode defines what the button shows and does while checking for updates. Refer to the
	 * documentation of the enum for more details.
	 *
	 * @accessors{
	 *	@memberAc{textMode}
	 * }
	 *
	 * @sa UpdateButton::TextMode
	 */
	property int textMode: UpdateButton.Cancel
	/*! @brief Specifies if the button makes it possible for the user to install updates
	 *
	 * @default{`true`}
	 *
	 * If set to true and after an update check, there are new updates, the button will show
	 * "Install" as text and when clicked start the update installation, according to the
	 * installMode and installScope properties via QtAutoUpdater::Updater::runUpdater().
	 *
	 * If set to false, the button will instead show the information, that updates are available.
	 * If clicked again, the button simply starts another updater check
	 *
	 * @accessors{
	 *	@memberAc{allowInstall}
	 * }
	 *
	 * @sa UpdateButton::installMode, UpdateButton::installScope, QtAutoUpdater::Updater::runUpdater
	 */
	property bool allowInstall: true
	/*! @brief The installation mode to be used to install updates
	 *
	 * @default{`Updater::InstallModeFlag::Parallel`}
	 *
	 * The install mode to be passed to Updater::runUpdater, if the allowInstall flag is set and the
	 * user launches an installation via the button.
	 *
	 * @accessors{
	 *	@memberAc{installMode}
	 * }
	 *
	 * @sa QtAutoUpdater::Updater::runUpdater, QtAutoUpdater::Updater::InstallModeFlag,
	 * UpdateButton::installScope
	 */
	property int installMode: Updater.Parallel
	/*! @brief The installation scope to be used to install updates
	 *
	 * @default{`Updater::InstallScope::PreferInternal`}
	 *
	 * The install scope to be passed to Updater::runUpdater, if allowInstall is set and the user
	 * launches an installation via the button.
	 *
	 * @accessors{
	 *	@memberAc{installScope}
	 * }
	 *
	 * @sa QtAutoUpdater::Updater::runUpdater, QtAutoUpdater::Updater::InstallModeFlag,
	 * UpdateButton::installMode
	 */
	property int installScope: Updater.PreferInternal

	//! [PRIVATE PROPERTY]
	property real _currentProgress: -1.0
	//! [PRIVATE PROPERTY]
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
				updater.runUpdater(installMode, installScope);
			break;
		case Updater.Checking:
			if (textMode == UpdateButton.Cancel)
				updater.abortUpdateCheck();
		default:
			break;
		}
	}
}
