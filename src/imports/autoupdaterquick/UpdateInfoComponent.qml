import QtQuick 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

UpdaterComponent {
	id: updateInfoComponent

	viewComponent: UpdateInfoPage {
	}

	readonly property Connections _connections: Connections {
		target: updater

		onCheckUpdatesDone: {
			if (result === Updater.NewUpdates)
				updateInfoComponent.triggerShow({})
		}
	}
}
