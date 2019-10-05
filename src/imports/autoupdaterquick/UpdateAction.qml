import QtQuick 2.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief A simple action for update checks
 *
 * @extends QtQuick.Controls.Action
 *
 * The UpdateAction is a ready-made "action" to check for updates. It is connected to an updater and
 * thus will automatically start checking for updates. It will also automatically disable itself
 * while checking for updates.
 *
 * @sa QtAutoUpdater::Updater, QtAutoUpdater::Updater::checkForUpdates,
 * @ref qtautoupdater_image_page "Image Page"
 */
Action {
	id: updateAction

	/*! @brief The updater to be used by the action to check for updates
	 *
	 * @default{`null`}
	 *
	 * @accessors{
	 *	@memberAc{updater}
	 * }
	 * @sa QtAutoUpdater::Updater
	 */
	property Updater updater: null

	text: qsTr("Check for updates")
	icon.name: "system-software-update"
	icon.source: "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/update.svg"
	enabled: updater && !updater.running

	onTriggered: updater.checkForUpdates()
}
