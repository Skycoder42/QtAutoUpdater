import QtQuick 2.13
import de.skycoder42.QtAutoUpdater.Core 3.0

/*! @brief [INTERNAL] Base class for components
 *
 * @extends QtQml.QtObject
 *
 * @warning This class is internal and cannot be instanciated. It only exists as common base for
 * the other components
 */
QtObject {
	id: updaterComponent

	/*! @brief The updater to be used by the component
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
	/*! @brief Specifies if the consumer of the component expects an item or a component
	 *
	 * @default{`false`}
	 *
	 * If set to true, the showItem() signal gets emitted with the already loaded item, as soon as
	 * the component was activated. If set to false, showComponent() is emitted instead. Use the
	 * property to specify which variant you need to consume the creation of a component.
	 *
	 * @accessors{
	 *	@memberAc{useAsComponent}
	 * }
	 *
	 * @sa UpdaterComponent::showComponent, UpdaterComponent::showItem
	 */
	property bool useAsComponent: false
	/*! @brief Specifies if the component is currently beeing loaded
	 *
	 * @default{`false`}
	 *
	 * This property can be used to show a busy indicator or something similar while the component
	 * is beeing loaded. Since the components are typically available locally, this can be ignored
	 * as loading is instant. However, when loading components from the network, this can be useful
	 * so the user does not think the app has stalled.
	 *
	 * @accessors{
	 *	@memberAc{loading}
	 *	@readonlyAc
	 * }
	 */
	readonly property bool loading: false

	/*! @brief Sets a callback to be called to close a created component
	 *
	 * @default{`null`}
	 *
	 * This must be a function with no parameters, i.e. `function() {...}`. It is called from any
	 * item created by this component if the item has to be closed. Thus, this method should remove
	 * the calling view from the GUI. Typically, this pops the view from a StackView or something
	 * similar. The following is an example for a stackview:
	 *
	 * @code{.qml}
	 * StackView {
	 *     id: stackView
	 *     ...
	 * }
	 *
	 * UpdaterComponent {
	 *     goBackCallback: stackView.pop
	 * }
	 * @endcode
	 *
	 * @attention You **have to** specifiy such a callback for the components to work properly. If
	 * not addend they are impossible to close, leading to a lockup of your GUI!!!
	 *
	 * @accessors{
	 *	@memberAc{goBackCallback}
	 * }
	 *
	 * @sa QtQuick.Controls.StackView
	 */
	property var goBackCallback: null

	/*! @brief Gets emitted to show this component in the GUI
	 *
	 * @param component The component to be loaded and shown
	 * @param params The parameters to be passed to the item to be created for it's initialization
	 *
	 * Catch this signal to actually show the GUI to be created by this component. You have to
	 * instancaite the component yourself, by using the given parameters. The following is an example
	 * for the StackView:
	 *
	 * @code{.qml}
	 * StackView {
	 *     id: stackView
	 *     ...
	 * }
	 *
	 * UpdaterComponent {
	 *     onShowComponent: stackView.push(component, params)
	 * }
	 * @endcode
	 *
	 * @attention This signal is only emitted if useAsComponent is set to true
	 *
	 * @sa UpdaterComponent::seAsComponent, UpdaterComponent::showItem, QtQuick.Controls.StackView
	 */
	signal showComponent(Component component, var params)
	/*! @brief Gets emitted to show this component in the GUI
	 *
	 * @param item The item to be shown
	 *
	 * Catch this signal to actually show the GUI to be created by this component. The component was
	 * already instanciated and initialized when passed via this signal, so you only have to add it
	 * to the GUI. The following is an example for the StackView:
	 *
	 * @code{.qml}
	 * StackView {
	 *     id: stackView
	 *     ...
	 * }
	 *
	 * UpdaterComponent {
	 *     onShowItem: stackView.push(item)
	 * }
	 * @endcode
	 *
	 * @attention This signal is only emitted if useAsComponent is set to false
	 *
	 * @sa UpdaterComponent::seAsComponent, UpdaterComponent::showItem, QtQuick.Controls.StackView
	 */
	signal showItem(Item item)

	//! @private [PRIVATE PROPERTY]
	property Component viewComponent

	//! @private
	function triggerShow(params) {
		params["updater"] = updater;
		params["goBackCallback"] = goBackCallback;

		if (useAsComponent)
			showComponent(viewComponent, params);
		else {
			let incubator = viewComponent.incubateObject(null, params, Qt.Asynchronous)
			if (incubator.status === Component.Ready)
				showItem(incubator.object);
			else {
				updaterComponent.loading = true;
				incubator.onStatusChanged = function(status) {
					switch (status) {
					case Component.Ready:
						updaterComponent.loading = false;
						showItem(incubator.object);
						break;
					case Component.Error:
						updaterComponent.loading = false;
						break;
					default:
						break;
					}
				}
			}
		}
	}
}
