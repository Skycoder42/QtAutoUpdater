import QtQuick 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

UpdaterComponent {
	id: updateInfoComponent

	viewComponent: UpdateInstallerPage {
	}

	readonly property Connections _connections: Connections {
		target: updater

		onShowInstaller: updateInfoComponent.triggerShow({
															 installer: installer
														 });
	}
}
