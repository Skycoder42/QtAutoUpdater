import QtQuick 2.13
import QtQuick.Controls 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief [INTERNAL] Base class for dialogs
 *
 * @extends QtQuick.Controls.Dialog
 *
 * @warning This class is internal and cannot be instanciated. It only exists as common base for
 * the other dialogs
 */
Dialog {
	id: dialogBase

	/*! @brief The updater to be used by the dialog
	 *
	 * @default{`null`}
	 *
	 * @accessors{
	 *	@memberAc{updater}
	 * }
	 *
	 * @sa QtAutoUpdater::Updater
	 */
	property Updater updater: null

	modal: true
	closePolicy: Dialog.CloseOnEscape
	anchors.centerIn: Overlay.overlay
	margins: 16
}
