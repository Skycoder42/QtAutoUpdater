import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Window 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0

ApplicationWindow {
	id: appWindow
	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")

	property Updater globalUpdater: QtAutoUpdater.createUpdater("playstore", {
																	"debug": false
																}, appWindow)

	StackView {
		id: stackView
		anchors.fill: parent

		initialItem: Page {
			header: ToolBar {
				Label {
					anchors.fill: parent
					text: qsTr("Android Updater Demo")
					verticalAlignment: Qt.AlignVCenter
					leftPadding: 16
					font.bold: true
					font.pointSize: label.font.pointSize * 1.5
				}
			}

			Label {
				id: label
				anchors.centerIn: parent
				text: qsTr("App Version: %1").arg(AppVersion)
			}

			UpdateButton {
				updater: globalUpdater
				anchors.top: label.bottom
				anchors.horizontalCenter: parent.horizontalCenter
				installMode: Updater.Parallel
				installScope: Updater.PreferExternal
			}
		}
	}

	UpdateInstallerComponent {
		updater: globalUpdater
		useAsComponent: true

		goBackCallback: stackView.pop

		onShowComponent: stackView.push(component, params)
	}
}
