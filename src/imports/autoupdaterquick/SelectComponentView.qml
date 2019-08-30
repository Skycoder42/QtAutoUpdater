import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

ScrollView {
	id: scrollView

	property UpdateInstaller installer: null

	readonly property string title: qsTr("Select Components")
	readonly property bool canGoNext: true
	readonly property bool canGoBack: true

	ListView {
		id: listView

		model: installer.componentModel

		delegate: CheckDelegate {
			id: delegate
			width: scrollView.width

			text: name
			checked: selected
			onCheckedChanged: selected = checked
		}
	}
}
