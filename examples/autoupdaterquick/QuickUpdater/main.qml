import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0

ApplicationWindow {
	visible: true
	width: 360
	height: 600
	title: qsTr("Hello World")

	property alias buttonOnly: btnOnlyBox.checked
	property Updater globalUpdater: QtAutoUpdater.createUpdater(examplePath)

	StackView {
		id: stackView
		anchors.fill: parent

		initialItem: Page {
			ColumnLayout {
				anchors.centerIn: parent

				CheckBox {
					id: btnOnlyBox
					text: qsTr("Update button only")
					checked: false
					Layout.minimumWidth: implicitWidth * 1.3
				}

				CheckBox {
					id: detailsBox
					text: qsTr("Detailed infos")
					checked: false
				}

				Button {
					id: sBtn
					enabled: !buttonOnly
					text: qsTr("GO!")
					onClicked: askDialog.open()
				}

				UpdateButton {
					updater: globalUpdater
				}
			}

			MouseArea {
				anchors.fill: parent
				z: -10

				acceptedButtons: Qt.RightButton
				onClicked: {
					testMenu.x = mouse.x;
					testMenu.y = mouse.y;
					testMenu.open();
				}

				Menu {
					id: testMenu

					UpdateAction {
						updater: globalUpdater
					}
				}
			}
		}
	}

	AskUpdateDialog {
		id: askDialog
		updater: buttonOnly ? null : globalUpdater
	}

	ProgressDialog {
		updater: buttonOnly ? null : globalUpdater
	}

	UpdateResultDialog {
		updater: buttonOnly ? null : globalUpdater
		showOnSuccess: !detailsBox.checked
	}

	UpdateInfoComponent {
		updater: buttonOnly || !detailsBox.checked ? null : globalUpdater
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
