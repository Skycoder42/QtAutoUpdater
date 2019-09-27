import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief A dialog to ask the user whether he wants to check for updates
 *
 * You can add this dialog to your UI and connect it to an updater. When you want to check for
 * updates, show this dialog via open(). If the user accepts the question, the updater will
 * automatically start the update check
 *
 * @sa QtAutoUpdater::Updater::checkForUpdates, @ref qtautoupdater_image_page "Image Page"
 */
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
