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

	property bool shouldRestart: false

	signal abortInstaller()

	readonly property int _canCancel: (installer.features & UpdateInstaller.CanCancel) != 0
	readonly property int _hasDetails: (installer.features & UpdateInstaller.DetailedProgress) != 0

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
					enabled: _hasDetails
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

					property real compProgress: progress
					property string compStatus: status

					contentItem: GridLayout {
						columns: 3

						Label {
							id: nameLabel
							Layout.fillWidth: true
							Layout.minimumWidth: implicitWidth
							text: name
							elide: Label.ElideRight
						}

						ProgressBar {
							id: progBar
							Layout.fillWidth: true
							Layout.columnSpan: 2
							Layout.minimumWidth: 0
							Layout.maximumWidth: implicitWidth
							from: 0.0
							to: 1.0
							value: Math.max(0.0, compProgress)
							indeterminate: compProgress < 0
						}

						Label {
							id: statusLabel
							Layout.fillWidth: true
							Layout.columnSpan: 2
							text: compStatus
							elide: Label.ElideMiddle
							font.italic: true
							font.pointSize: nameLabel.font.pointSize * 0.9
						}

						Label {
							id: percentLabel
							Layout.minimumWidth: implicitWidth
							text: qsTr("%n%", "", compProgress * 100)
							horizontalAlignment: Qt.AlignRight
							font.italic: true
							font.pointSize: nameLabel.font.pointSize * 0.9
							visible: !progBar.indeterminate
						}
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

		onAccepted: installView.abortInstaller()

		Label {
			id: contentLabel
			anchors.fill: parent

			wrapMode: Label.Wrap

			text: qsTr("Do you want to check for updates now?")
		}
	}
}
