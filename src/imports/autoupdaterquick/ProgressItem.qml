import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13

GridLayout {
	id: progressItem
	columns: 2
	columnSpacing: 8

	property real progress: -1.0
	property alias status: statusLabel.text

	readonly property bool indeterminate: progressItem.progress < 0

	FontMetrics {
		id: metrics
		font: percentLabel.font
	}

	ProgressBar {
		id: progressBar
		Layout.minimumHeight: metrics.height
		Layout.fillWidth: true
		Layout.columnSpan: progressItem.indeterminate ? 2 : 1

		from: 0.0
		to: 1.0
		value: progressItem.progress
		indeterminate: progressItem.indeterminate
	}

	Label {
		id: percentLabel
		Layout.minimumHeight: metrics.height
		Layout.minimumWidth: metrics.boundingRect(qsTr("%n%", "", 100)).width
		Layout.preferredWidth: Layout.minimumWidth

		visible: !progressItem.indeterminate
		text: qsTr("%n%", "", progressItem.progress * 100)
		horizontalAlignment: Qt.AlignRight
	}

	Label {
		id: statusLabel
		Layout.columnSpan: 2
		Layout.fillWidth: true
		Layout.fillHeight: true
		Layout.minimumHeight: metrics.height

		font.italic: true
		elide: Text.ElideMiddle
	}
}
