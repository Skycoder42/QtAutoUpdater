import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0

ApplicationWindow {
	visible: true
	width: 360
	height: 600
	title: qsTr("Hello World")

	property Updater globalUpdater: QtAutoUpdater.createUpdater("/home/sky/Programming/QtLibraries/QtAutoUpdater/examples/autoupdatergui/SimpleUpdaterGui/example.conf")

	StackView {
		id: stackView
		anchors.fill: parent

		initialItem: Page {
			Button {
				anchors.centerIn: parent
				text: qsTr("GO!")
				onClicked: askDialog.open()
			}
		}
	}

	AskUpdateDialog {
		id: askDialog
		updater: globalUpdater
	}

	ProgressDialog {
		updater: globalUpdater
	}

	UpdateResultDialog {
		updater: globalUpdater
	}

	UpdateInfoComponent {
		updater: globalUpdater
		useAsComponent: true

		goBackCallback: stackView.pop

		onShowComponent: stackView.push(component, params)
	}

	UpdateInstallerComponent {
		updater: globalUpdater
		useAsComponent: true

		goBackCallback: stackView.pop

		onShowComponent: stackView.push(component, params)
	}
}
