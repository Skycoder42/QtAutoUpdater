import QtQuick 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief A component that creates information views with detailed infos about updates
 *
 * This class should be placed in your GUI as a view factory. It will emit the showItem() or
 * showComponent() signals (depending on useAsComponent) and you have to catch this signal to
 * actually show the created UI. The UI is a custom QQuickItem and must be shown as full screen view.
 *
 * It is internally connected to the QtAutoUpdater::Updater::checkDone() signal of the used updater
 * and will automatically emit the signal as soon as new updates are available.
 *
 * The following example shows how to realize this with a StackView as view container:
 *
 * @code{.qml}
 * StackView {
 *     id: stackView
 *     anchors.fill: parent  // parent should be the root window
 *     ...
 * }
 *
 * UpdateInfoComponent {
 *     id: component
 *     updater: QtAutoUpdater.createUpdater(...)
 *     useAsComponent: true
 *
 *     goBackCallback: stackView.pop
 *     onShowItem: stackView.push(item)
 * }
 * @endcode
 *
 * @sa UpdaterComponent, UpdateResultDialog, UpdaterComponent::showItem,
 * UpdaterComponent::showComponent, UpdaterComponent::useAsComponent, QtQuick.Controls.StackView,
 * QtAutoUpdater::Updater::checkDone, @ref qtautoupdater_image_page "Image Page"
 */
UpdaterComponent {
	id: updateInfoComponent

	viewComponent: UpdateInfoPage {
	}

	//! [PRIVATE PROPERTY]
	readonly property Connections _connections: Connections {
		target: updater

		onCheckUpdatesDone: {
			if (result === Updater.NewUpdates)
				updateInfoComponent.triggerShow({})
		}
	}
}
