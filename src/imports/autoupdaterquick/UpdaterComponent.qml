import QtQuick 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

QtObject {
	id: updaterComponent

	property Updater updater: null
	property bool useAsComponent: false
	property bool loading: false

	property var goBackCallback: null

	signal showComponent(Component component, var params)
	signal showItem(Item item)

	property Component viewComponent

	function triggerShow(params) {
		params["updater"] = updater;
		params["goBackCallback"] = goBackCallback;

		if (useAsComponent)
			showComponent(viewComponent, params);
		else {
			let incubator = viewComponent.incubateObject(null, params, Qt.Asynchronous)
			if (incubator.status === Component.Ready)
				showItem(incubator.object);
			else {
				updaterComponent.loading = true;
				incubator.onStatusChanged = function(status) {
					switch (status) {
					case Component.Ready:
						updaterComponent.loading = false;
						showItem(incubator.object);
						break;
					case Component.Error:
						updaterComponent.loading = false;
						break;
					default:
						break;
					}
				}
			}
		}
	}
}
