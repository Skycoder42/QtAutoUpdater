import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

Item {
	id: installView

	property UpdateInstaller installer: null

	readonly property string title: qsTr("Install Updates")
	property bool canGoNext: false
	property bool canGoBack: false

	property bool shouldRestart: value

	readonly property int _canCancel: (installer.features & UpdateInstaller.CanCancel) != 0

	state: "ready"
	states: [
		State {
			name: "ready"
			PropertyChanges {
				target: installView
				canGoBack: true
			}
		},
		State {
			name: "installing"
			PropertyChanges {
				target: installButton
				enabled: false
			}
		},
		State {
			name: "installing-cancel"
			PropertyChanges {
				target: installButton
				text: qsTr("Cancel")
				highlighted: false
			}
		},
		State {
			name: "cancelling"
			PropertyChanges {
				target: installButton
				text: qsTr("Cancel")
				highlighted: false
				enabled: false
			}
		},
		State {
			name: "done"
			PropertyChanges {
				target: installButton
				enabled: false
			}
			PropertyChanges {
				target: installView
				canGoNext: true
			}
		},
		State {
			name: "error"
			PropertyChanges {
				target: installButton
				enabled: false
			}
		}
	]

	Connections {
		target: installer

		onUpdateGlobalProgress: {
			progressItem.progress = progress;
			progressItem.status = status;
		}

		onInstallSucceeded: {
			installView.shouldRestart = shouldRestart;
			installView.state = "done";
		}

		onInstallFailed: {
			installView.state = "error";
			errorDialog.showError(errorMessage);
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Pane {
			id: progressPane
			Layout.fillWidth: true

			GridLayout {
				anchors.fill: parent
				columns: 3

				ProgressItem {
					id: progressItem
					Layout.columnSpan: 3
					Layout.fillWidth: true
				}

				Button {
					id: detailsButton
					text: qsTr("Details…")
					checkable: true
				}

				Item {
					Layout.fillWidth: true
				}

				Button {
					id: installButton
					text: qsTr("Install")
					highlighted: true

					onClicked: {
						if (installView.state == "ready") {
							installer.startInstall();
							installView.state = _canCancel ? "installing-cancel" : "installing";
						} else if (installView.state == "installing-cancel") {
							installer.cancelInstall();
							installView.state = "cancelling";
						}
					}
				}
			}
		}

		MenuSeparator {
			Layout.fillWidth: true
			padding: 0
		}

		ScrollView {
			id: scrollView
			Layout.fillWidth: true
			Layout.fillHeight: true
			visible: detailsButton.checked

			ListView {
				id: listView

				model: installer.progressModel

				delegate: ItemDelegate {
					id: delegate
					width: scrollView.width

					text: name
					property real compProgress: progress
					property string compStatus: status

					indicator: ProgressItem {
						progress: compProgress
						status: compStatus

						width: Math.min(delegate.width - delegate.padding * 3 - delegate.contentItem.implicitWidth, implicitWidth)
						anchors.right: parent.right
						anchors.rightMargin: delegate.rightPadding
						anchors.verticalCenter: parent.verticalCenter
					}
				}
			}
		}

		Item {
			Layout.preferredHeight: 0
			Layout.fillHeight: true
		}
	}

	DialogBase {
		id: errorDialog
		visible: false

		function showError(errorMessage) {
			text = errorMessage;
			open();
		}

		title: qsTr("Installation failed!")
		property alias text: contentLabel.text
		standardButtons: Dialog.Ok

		onAccepted: Qt.quit(); // TODO exit installer only

		Label {
			id: contentLabel
			anchors.fill: parent

			wrapMode: Label.Wrap

			text: qsTr("Do you want to check for updates now?")
		}
	}
}
