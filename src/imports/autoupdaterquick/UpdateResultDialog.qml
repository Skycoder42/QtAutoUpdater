import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief A dialog to show the result of an update check
 *
 * You can add this dialog to your UI and connect it to an updater. When an update check has
 * completed, this dialog will show up, based on it's configuration, and inform the user about the
 * result. It can also optionally allow him to install updates or do so automatically.
 *
 * @sa QtAutoUpdater::Updater, @ref qtautoupdater_image_page "Image Page"
 */
DialogBase {
	id: resultDialog
	visible: false

	/*! @brief Show the dialog on a successfull update check
	 *
	 * @default{`true`}
	 *
	 * If the check for updates succeeded, the dialog will only open up if this property is set
	 * to true. However, a few other properties limit this further. showOnNoUpdates and
	 * autoRunUpdater can lead to the dialog not beeing shown, even on success.
	 *
	 * If autoRunUpdater was not enabled (set to false), the user will be presented with the options
	 * to install updates, based on which variants the backend provides. Thus the user can decide if
	 * he wants to install updates now, on exit or later.
	 *
	 * @accessors{
	 *	@memberAc{showOnSuccess}
	 * }
	 *
	 * @sa UpdateResultDialog::showOnNoUpdates, UpdateResultDialog::showOnFailure,
	 * UpdateResultDialog::autoRunUpdater
	 */
	property bool showOnSuccess: true
	/*! @brief Show the dialog on a failed update check
	 *
	 * @default{`true`}
	 *
	 * If the check for updates failed with an error, the dialog will only open up if this property
	 * is set to true. It will not show an error message, but inform the user that an error occured.
	 *
	 * @accessors{
	 *	@memberAc{showOnFailure}
	 * }
	 *
	 * @sa UpdateResultDialog::showOnSuccess
	 */
	property bool showOnFailure: true
	/*! @brief Show the dialog on a successfull update check, even if no updates are available
	 *
	 * @default{`true`}
	 *
	 * If the check for updates succeeded and the showOnSuccess property is set, this property
	 * controls what happens if no updates are available. If set to true, the user is informed that
	 * there are no new updates. If set to false, nothing is shown.
	 *
	 * @accessors{
	 *	@memberAc{showOnNoUpdates}
	 * }
	 *
	 * @sa UpdateResultDialog::showOnSuccess
	 */
	property bool showOnNoUpdates: true
	/*! @brief Specifies if the dialog should automatically start an installation
	 *
	 * @default{`false`}
	 *
	 * If set to true, on a successful update, an installation will automatically be started.
	 * The autoInstallMode and installScope properties are used as arguments for
	 * QtAutoUpdater::Updater::runUpdater() and thus determine how the installer is started.
	 *
	 * If a parallel installation is launched, the dialog itself will not show anything. If an
	 * installation is planned to be run on exit, an information is shown to the user, but only if
	 * the showOnSuccess property was set to true. If the dialog failes to start the installer and
	 * showOnFailure is set to true, an error message is shown informing the user that updates are
	 * available, but could not be installed.
	 *
	 * @accessors{
	 *	@memberAc{autoRunUpdater}
	 * }
	 *
	 * @sa UpdateResultDialog::showOnSuccess, UpdateResultDialog::showOnFailure,
	 * UpdateResultDialog::autoInstallMode, UpdateResultDialog::installScope,
	 * QtAutoUpdater::Updater::runUpdater
	 */
	property bool autoRunUpdater: false
	/*! @brief The install mode to use for automatic update installation
	 *
	 * @default{`QtAutoUpdater::Updater::InstallModeFlag::Parallel`}
	 *
	 * This is passed to QtAutoUpdater::Updater::runUpdater() for automatic updates, but not used if
	 * the user selects update installation manually.
	 *
	 * @accessors{
	 *	@memberAc{autoInstallMode}
	 * }
	 *
	 * @sa UpdateResultDialog::autoRunUpdater, UpdateResultDialog::installScope,
	 * QtAutoUpdater::Updater::runUpdater, QtAutoUpdater::Updater::InstallModeFlag
	 */
	property int autoInstallMode: Updater.Parallel
	/*! @brief The install scope to use for update installation
	 *
	 * @default{`QtAutoUpdater::Updater::InstallScope::PreferInternal`}
	 *
	 * This is passed to QtAutoUpdater::Updater::runUpdater() for automatic any update installation
	 * started via this dialog. It applies for automatic and for user triggered updates and sets the
	 * prefered way of installing.
	 *
	 * @accessors{
	 *	@memberAc{installScope}
	 * }
	 *
	 * @sa UpdateResultDialog::autoRunUpdater, UpdateResultDialog::autoInstallMode,
	 * QtAutoUpdater::Updater::runUpdater, QtAutoUpdater::Updater::InstallScope
	 */
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
					if (updater.runUpdater(autoInstallMode, installScope))
						showInfo = showInfo && updater.runOnExit;
					else if(showOnFailure) {
						contentLabel.state = "run-updater-fail";
						resultDialog.open();
						return;
					}
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
			},
			State {
				name: "run-updater-fail"
				PropertyChanges {
					target: contentLabel
					text: qsTr("Updates are available, but was not able to start the update installer!");
				}
			}

		]
	}
}
