# Porting guide
The following guide will help you to convert an application that uses version 2.1.5 of the library to the new
version 3.0.0. This was a major update and thus introduces code changes that have to be manually adjusted. The
update mostly added features and changed the internal structure, but existing classes had their APIs cleaned up
as well.

> This guide will only handle changes to existing APIs, i.e. the core and widgets modules with the QtIFW plugin. Any other new additions are not referenced here.

## General API changes
The core change from 2 to 3 are two main things. First is, that the library now supports more than just QtIFW. It
now supports 6 different backends, in the form of plugins. This does not really matter for this guide. Only how you
deploy the application has sligthly changed. Second, the module now allows more sofisticated installations. It can
run installations in parallel and track their progress. This too, does not really affect QtIFW, as it only launches
an external installer. However, on Linux and macOs, it is now possible to run the maintenancetool in parallel to the
running application.

Information about the plugin for QtIFW and it specialities can be found on the
@ref qtautoupdater_backends "backends page" in the documentation.

## Porting QtAutoUpdaterCore
The following changes have been made to the core module. They are mostly cosmetic changes, i.e. changing how the
API is presented. The are listed in the following ordered by classes.

### Changes to UpdateInfo
The UpdateInfo class has been moved out of Updater is now called QtAutoUpdater::UpdateInfo. Is has been converted to
a fully fledged gadget. Instead of members, it has now equally named properties with get and set accessors. In
addition, there are comparators and functions to use them as key to hashes.

The `size` member has been completly removed, as part of the generalization that happend to the library. You can
still access it, via the newly added `data` property:
@code{.cpp}
auto size = info.data()["size"].toULongLong();
@endcode
The size is also the secondary info property for QtIFW. See QtAutoUpdater::UpdaterBackend::secondaryInfo.

### Changes to AdminAuthoriser
This class has always been more of a side-class. Instead of a class it is now a namespace and provides two methods.
The `hasAdminRights` method has been removed and replaced with QtAutoUpdater::AdminAuthoriser::needsAdminPermission.
That method instead tries to guess if the execution of the given executable needs an elevation of rights first, or
if it can be executed in the current context. `executeAsAdmin` has not been changed, but now allows you to add the
working directory as parameter (ignored on macOs).

### Changes to Updater
As the primary class of the module, this class has changed the most. While is functionality stayed the same - to
check for updates and launch the installation - the API has been improved and generalized. Also, since now plugins
are used, the configuration is different

#### Creation
Instead of creating it via the constructor, there is now a factory method - QtAutoUpdater::Updater::create. It
takes a backend and a configuration and creates the updater using that backend from it. To create an updater with a
maintenancetool, use the following code:

@code{.cpp}
auto updater = QtAutoUpdater::Updater::create("qtifw", {
	{"path", pathToTheMaintenancetool}
}, parent);
@endcode

To create an updater instance with the default path, simply leave out the configuration. There are a few other
parameters you can use here. There are partially explained in other parts of this guide - but you can also take a
look at @ref qtautoupdater_backends_qtifw. You can also use a configuration file. It would look like this:

@code{.ini}
[General]
backend=qtifw
path=pathToTheMaintenancetool
...
@endcode

#### Status report
The updater now uses the QtAutoUpdater::Updater::state property to track the state. The
QtAutoUpdater::Updater::running property stayed, but is simply a wrapper around that state to simplify it down to
the information, whether the updater is doing something or not. All the accessors directly linked with the
maintenancetool have been removed. This includes the members `maintenanceToolPath`, `exitedNormally`, `errorCode`
and `errorLog`. This information is now logged internally and cannot be accessed anymore.

#### Checking for updates
The method to start an update check stayed the same: QtAutoUpdater::Updater::checkForUpdates - but it now returns
void instead of bool, as the state can be used to track the progress. The QtAutoUpdater::Updater::progressChanged
signal has been added to provide a more detailed progress for supported plugins. QtIFW does not provide those
details, so you can ignore the signal. Once the update check is done, the QtAutoUpdater::Updater::checkUpdatesDone signal is still beeing emitted, but it
now only has the status as parameter instead of two booleans. The update details can still be accessed via the
QtAutoUpdater::Updater::updateInfo property.

It is also possible to add additional arguments to the update check run of the maintenancetool. To do so, use the
`extraCheckArgs` in the configuration to specify those arguments:

@code{.code}
{
	{"extraCheckArgs", "someArg=tree"}
}
@endcode

#### Installing updates
The method to start an installation has been renamed from `runUpdaterOnExit` to QtAutoUpdater::Updater::runUpdater
and now takes two arguments to specify how updates should be installed. There are various options, but only two of
them are viable:

- QtAutoUpdater::Updater::runUpdater(QtAutoUpdater::Updater::InstallModeFlag::OnExit)
- QtAutoUpdater::Updater::runUpdater(QtAutoUpdater::Updater::InstallModeFlag::Parallel)

The first one works for all platforms and exhibits the same behaviour as the old version. It schedules the installer
to be executed on exit of the running application. The second one only works on Linux and macOs and launches the
installer immediatly, in parallel to the running application. For this variant, you can track the installation via
the state and by waiting for the QtAutoUpdater::Updater::installDone signal.

If you want to specify additional arguments to the installation, use the configuration. The following parameters
can be added. `silent` will make the updater run in background, `extraInstallArgs` allows you to pass any additional
arguments you desire to the installer and `runAsAdmin` can be used to specify if the installer should be run as
admin or not. Leaving that one out will make the backend use the QtAutoUpdater::AdminAuthoriser to guess the
correct value. The code below shows how the old constants are mapped to parameters in the configuration:

@code{.code}
// NormalUpdateArguments
{}
// PassiveUpdateArguments
{
	{"extraInstallArgs", "skipPrompt=true"}
}
// HiddenUpdateArguments
{
	{"silent", true}
}
@endcode

#### Other changes
The remaining changes are mostly stuff that has to do with the new plugin architecture and other small additions.
Read the documentation for more details. Scheduling update checks is also still possible and now even supports use
of the std::chrono classes.

### Added classes
The remaining changes to the module are mostly added classes that have to do with the new plugin architecture.
Again, check the documentation for more details about these classes.

## Porting QtAutoUpdaterGui
The first thing to say here is, that the module has been renamed. It is now called *QtAutoUpdaterWidgets*, as it
only creates a widgets GUI. For QML, there now is the `de.skycoder42.QtAutoUpdater.Quick` module you can import and
use. Refer to the documentation of that namespace and the README for more details on how to use it. There haven't
been any big changes functionality wise to this module. It still uses the controller to show a GUI and provides the
UpdateButton as a dynamic button. However, the way these classes are used has changed and adjusted to the changes
in the core module. No classes have been added or removed.

### Changes to UpdateController
The controllers API has been changed quite a bit. A lot of functionality that was duplicated from the internally
used updater has been removed, as it can now be completly controlled via the updater. The GUIs that are shown by
the controller however have not been changed.

#### Construction and window affiliation
Instead of using a maintenancetool, you now have to create the controller by passing it an updater. The controller
does not take ownership of the updater, which is why the parenting has been united. If you create the controller
with a QWidget as parent, all windows will be parented to that widget as well. If any other QObject is used as
parent, the windows will be parentless toplevel windows instead. If the controller gets destroyed early, the updater
stays alive and can still trigger an installer on exit. Thus, it is recommended to parent the controller correctly.

In addition, the controller will now only show the GUIs if the parent window is visible (or minimized). A windows
counts as visible even if hidden behind other windows or on another desktop. This allows you to add multiple
controllers to different GUIs of your application, without the interferring, as long as only one of those windows
is visible at a times.

Finally, the QtAutoUpdater::UpdateController::createUpdateAction has been made static, the the action only requires
a reference to an updater, not to the controller anymore. Otherwise, the action is still the same. It will however
call QtAutoUpdater::Updater::checkForUpdates instead of using a controller, which means it is not possible to have
an action with asklevel anymore. Any other level still works, as explained in the next section.

#### Auto attachment to updaters
Unlike before, the controller now "attaches" to the updater and can show a GUI in any state of the update check. If
you for example create a controller while already checking for updates, there won't be a progress, but as soon as
the check has finished, a result dialog will be shown (if enabled by display level).

Because of this change, the `running` property of the controller has been removed. Use the updater instead to check
if it is running or not.

#### Setting the display level and running the controller
Another change has been made to the display level handling. The display level is now a property,
QtAutoUpdater::UpdateController::displayLevel. This property can be set and changed even during a running update
check and takes effect as soon as the next dialog is shown. It is know global for all checks run via this
controller. It is still possible to pass the level to the QtAutoUpdater::UpdateController::start method, but this
will only set the property and then call `start` without any parameters.

To run the controller, you should still call `start`, as it will show the ask dialog if wished before checking for
updates. However, it is also possible to use QtAutoUpdater::Updater::checkForUpdates instead, as the controller is
attached to the updater. This will work for all levels but the ask level, as the ask dialog will never be shown when
an update check is started via the updater.

Use the properties and signals of the QtAutoUpdater::Updater to track the progress. All such methods have been
removed from the controller, as they are not needed there anymore.

#### Removed members
There are quite a few properties and methods that have been removed. Those have been mostly removed because the
same functionality is already beeing provided by the underlying updater. The removed members are:
`maintenanceToolPath`, `running`, `runAsAdmin`, `updateRunArgs`, `scheduleUpdate` and `cancelScheduledUpdate`.

In the GUI, the run as admin checkbox has been removed. You should get this information programatically, and not
by prompting the user, as the user typically has no clue about this. You can still specify it, by using the
`runAsAdmin` configuration value.

### Changes to UpdateButton
The update button has seen changes similar to the controller. The main difference is, that it now only needs an
updater instead of a controller, and that it supports a more sofisticated UI, including a state that is shown while
installing parallel updates for Linux or macOs. The rest of it's GUI however has stayed the same.

The fact that the button now uses an QtAutoUpdater::Updater only also means, that there is no display level anymore.
Again, this is not problematic as any controller will automatically show required GUIs, even if completely detached
from the button, as long as they use the same updater.

Another thing that changed is, that you can now trigger an installation via the button, if updates are available.
The QtAutoUpdater::UpdateButton::mode property controlls whether this is possible and how the button operates in
general. If allowed and a user triggers the installation, the QtAutoUpdater::UpdateButton::installMode and
QtAutoUpdater::UpdateButton::installScope properties are used as arguments for the
QtAutoUpdater::Updater::runUpdater method call.

## Final remarks
As you can see from this guide, there have been quite a few changes. Most of them however are nothing more then
sligthly more complex refactorings. Using this document you should be able to easily port any application from the
old to the new version. If you however still have some questions, simply use the
[GitHub Issues](https://github.com/Skycoder42/QtAutoUpdater/issues) to ask for help. I am aware that such a change
can be challenging and I am always willing to provide any help I can.
