import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

Pane {
	id: successView

	property UpdateInstaller installer: null

	readonly property string title: qsTr("Installation successful!")
	readonly property bool canGoNext: true
	readonly property bool canGoBack: false
	property alias shouldRestart: restartBox.visible
	readonly property bool doRestart: shouldRestart && restartBox.checked

	ColumnLayout {
		anchors.fill: parent

		CheckBox {
			id: restartBox
			Layout.fillWidth: true
			visible: false
			checked: true
			text: qsTr("Restart the application")
		}

		Label {
			Layout.fillWidth: true
			text: qsTr("Press the X-Button to close the installer…")
			wrapMode: Label.Wrap
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}
	}
}
