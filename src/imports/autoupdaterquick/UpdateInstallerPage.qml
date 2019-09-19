import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0
import de.skycoder42.QtAutoUpdater.Quick.internal 3.0

Page {
	id: updateInfoPage

	property Updater updater: null
	property UpdateInstaller installer: null
	onInstallerChanged: installer.parent = updateInfoPage

	property var goBackCallback: null

	readonly property int _pageOffset: !installer || ((installer.features & UpdateInstaller.SelectComponents) != 0) ? 0 : 1
	readonly property int _dummyStackDepth: pageStack.depth + _pageOffset

	function doGoBack() {
		if (goBackCallback) {
			QtAutoUpdaterQuick.destroy(installer);
			goBackCallback();
		}
	}

	header: ToolBar {
		id: toolBar

		RowLayout {
			anchors.fill: parent
			spacing: 0

			Label {
				Layout.fillWidth: true
				Layout.fillHeight: true
				text: pageStack.currentItem.title
				font.bold: true
				font.pointSize: toolBar.font.pointSize * 1.2
				verticalAlignment: Qt.AlignVCenter
				leftPadding: 16
			}
		}
	}

	StackView {
		id: pageStack
		anchors.fill: parent

		Component {
			id: selectComponentsComponent
			SelectComponentView {
				installer: updateInfoPage.installer
			}
		}

		Component {
			id: installComponent
			InstallView {
				installer: updateInfoPage.installer
				onAbortInstaller: doGoBack()
			}
		}

		Component {
			id: successComponent
			SuccessView {
				installer: updateInfoPage.installer
			}
		}

		initialItem: _pageOffset == 0 ? selectComponentsComponent : installComponent
	}

	DialogBase {
		id: eulaDialog

		property var eulaList: []
		property var currentId
		property alias text: contentLabel.text
		property bool required: false

		title: required ? qsTr("Accept EULA?") : qsTr("EULA provided")
		standardButtons: required ? Dialog.Yes | Dialog.No : Dialog.Ok

		onAccepted: {
			if (required)
				installer.eulaHandled(currentId, true);
			if (eulaList.length > 0)
				Qt.callLater(eulaDialog.nextEula);
		}

		onRejected: {
			if (required)
				installer.eulaHandled(currentId, false);
		}

		function nextEula() {
			currentId = eulaList[0].id;
			text = eulaList[0].htmlText;
			required = eulaList[0].required;
			eulaList.splice(0, 1);
			eulaDialog.open();
		}

		Label {
			id: contentLabel
			anchors.fill: parent

			wrapMode: Label.Wrap
		}

		Connections {
			target: installer
			onShowEula: {
				eulaDialog.eulaList.push({
											 id: id,
											 htmlText: htmlText,
											 required: required,
										 });
				if (!eulaDialog.opened)
					eulaDialog.nextEula(true);
			}
		}
	}

	footer: RowLayout {
		ToolButton {
			enabled: pageStack.currentItem.canGoBack && (pageStack.depth > 1 || goBackCallback)

			display: ToolButton.IconOnly
			icon.source: "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/before.svg"
			icon.name: "go-previous"

			ToolTip.visible: pressed
			ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
			ToolTip.text: qsTr("Go back to the previous page")

			onClicked: {
				if (pageStack.depth > 1)
					pageStack.pop();
				else
					doGoBack();
			}
		}

		Item {
			Layout.fillWidth: true
		}

		PageIndicator {
			count: 3 - _pageOffset
			currentIndex: pageStack.depth - 1
		}

		Item {
			Layout.fillWidth: true
		}

		ToolButton {
			enabled: pageStack.currentItem.canGoNext

			display: ToolButton.IconOnly
			icon.source: _dummyStackDepth == 3 ?
							 "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/close.svg" :
							 "qrc:/de/skycoder42/QtAutoUpdater/Quick/icons/next.svg"
			icon.name: _dummyStackDepth == 3 ? "view-close" : "go-next"

			ToolTip.visible: pressed
			ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
			ToolTip.text: qsTr("Go to the next page")

			onClicked: {
				switch (_dummyStackDepth) {
				case 1:
					pageStack.push(installComponent);
					break;
				case 2:
					pageStack.push(successComponent, {
									   shouldRestart: pageStack.currentItem.shouldRestart
								   });
					break;
				case 3:
					if (pageStack.currentItem.doRestart)
						installer.restartApplication();
					doGoBack();
					break;
				default:
					break;
				}
			}
		}
	}
}
