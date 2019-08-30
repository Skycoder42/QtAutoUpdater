import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0
import de.skycoder42.QtAutoUpdater.Quick 3.0

Page {
	id: updateInfoPage

	property Updater updater: null
	property UpdateInstaller installer: null

	property var goBackCallback: null

	// TODO reparent installer!

	readonly property int _pageOffset: (installer.features & UpdateInstaller.SelectComponents) != 0 ? 0 : 1
	readonly property int _dummyStackDepth: pageStack.depth + _pageOffset

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
				onAbortInstaller: {
					if (goBackCallback)
						goBackCallback();
				}
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
				else if (goBackCallback)
					goBackCallback();
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
					if (goBackCallback)
						goBackCallback();
					break;
				default:
					break;
				}
			}
		}
	}
}
