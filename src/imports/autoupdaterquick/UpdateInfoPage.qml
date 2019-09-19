import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0
import de.skycoder42.QtAutoUpdater.Quick.internal 3.0

Page {
	id: updateInfoPage

	property Updater updater: null
	property int installScope: Updater.PreferInternal

	property var goBackCallback: null

	readonly property bool canTrigger: (updater.features & UpdaterBackend.TriggerInstall) != 0
	readonly property bool canParallel: (updater.features & UpdaterBackend.ParallelTrigger) != 0 || (updater.features & UpdaterBackend.PerformInstall) != 0
	readonly property int actionDisplayMode: canTrigger && canParallel && updateInfoPage.width < updateInfoPage.height ? ToolButton.IconOnly : ToolButton.TextBesideIcon

	header: ToolBar {
		id: toolBar

		RowLayout {
			anchors.fill: parent
			spacing: 0

			ToolButton {
				id: backButton
				visible: updateInfoPage.goBackCallback

				display: ToolButton.IconOnly
				icon.source: "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/back.svg"
				icon.name: "arrow-left"

				ToolTip.visible: pressed
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Go back without installing updates")

				onClicked: updateInfoPage.goBackCallback()
			}

			Label {
				Layout.fillWidth: true
				Layout.fillHeight: true
				text: qsTr("New Updates!")
				font.bold: true
				font.pointSize: toolBar.font.pointSize * 1.2
				verticalAlignment: Qt.AlignVCenter
				leftPadding: backButton.visible ? 8 : 16
			}

			ToolButton {
				visible: canTrigger

				display: actionDisplayMode
				icon.source: "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/update_later.svg"
				icon.name: "download-later"
				text: qsTr("Install on exit")

				ToolTip.visible: pressed
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Install the new updates as soon as you exit the app")

				onClicked: {
					goBackCallback();
					let ok = updater.runUpdater(Updater.ForceOnExit, installScope);
					if (!ok)
						errorDialog.open();
				}
			}

			ToolButton {
				visible: canParallel || canTrigger

				display: actionDisplayMode
				icon.source: "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/update.svg"
				icon.name: "update-none"
				text: qsTr("Install now")

				ToolTip.visible: pressed
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Install the new updates immediatly.") + (canParallel ? "" : qsTr(" This will exit the application to start the installer!"))

				onClicked: {
					goBackCallback();
					let ok = updater.runUpdater(Updater.Parallel, installScope);
					if (!ok)
						errorDialog.open();
					else if (updater.runOnExit)
						Qt.quit();
				}
			}
		}
	}

	ScrollView {
		id: scrollView
		anchors.fill: parent

		ListView {
			id: listView

			model: UpdateInfoModel {
				secondaryInfo: QtAutoUpdaterQuick.secondaryInfoKey(updater)
				updateInfos: updater.updateInfo
			}

			delegate: ItemDelegate {
				id: delegate
				width: scrollView.width

				text: name

				indicator: Label {
					text: qsTr("Version: %1").arg(version)
					anchors.right: parent.right
					anchors.rightMargin: delegate.rightPadding
					anchors.verticalCenter: parent.verticalCenter
				}

				ToolTip.visible: listView.model.secondaryInfo != "" && pressed
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("%1: %2").arg(QtAutoUpdaterQuick.secondaryInfoDisplayName(updater)).arg(secondaryInfo)
			}
		}
	}

	DialogBase {
		id: errorDialog
		visible: false

		title: qsTr("Install updates failed!")
		standardButtons: Dialog.Ok

		Label {
			id: contentLabel
			anchors.fill: parent

			text: qsTr("Unable to install updates! Failed to launch the update installer.")
			wrapMode: Label.Wrap
		}
	}
}
