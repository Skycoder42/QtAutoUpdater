# QtAutoUpdater

A library to automatically check for updates and install them. This repository includes:
 - A library with the basic updater (without any GUI) and an update scheduler
 - A second library that requires the first one and adds basic GUI features

Github repository: https://github.com/Skycoder42/QtAutoUpdater

## Main Features
### Core Library
 - Automatic Check for updates using the maintenancetool
 - Can automatically run the maintenancetool after the application finished
   - To run as admin/root, either use the GUI or implement it yourself (via an interface)
 - UpdateScheduler:
   - Supports different Schedule-Types:
     - Timepoint, with (optional) repetition of e.g. the year/month/...
     - Timespans, with or without repetitions
     - Abstact lists of update tasks
   - Supports custom Schedule-types (via an interface)
   - can store unfinished tasks and complete them the next time the application runs, if they are valid

### GUI Library
 - Requires Widgets (no direct QML-support, and I'm not intending to create one. If you want to use the updater in QML, do the connection yourself!)
 - Automated controller to guide the user through the check-for-updates process
   - customizable: you can decide what to show
   - extended information dialog to show basic information about the update
 - "Run as Admin/root" fully supported on all 3 platforms
 - UpdateAction -> a QAction that starts the updater
 - UpdatePanel -> a button with a busy-indicator to to the same

#### Screenshots
Here some sample screenshots of the gui (The rocket of the information dialog is the "application icon" and depends on your application)

| Dialog Sample      | Windows                                   | Mac                                       | X11                                       |
|--------------------|-------------------------------------------|-------------------------------------------|-------------------------------------------|
| Progress Dialog    | ![](./doc/images/win/dialog_progress.png) | ![](./doc/images/mac/dialog_progress.png) | ![](./doc/images/x11/dialog_progress.png) |
| Information Dialog | ![](./doc/images/win/dialog_info.png)     | ![](./doc/images/mac/dialog_info.png)     | ![](./doc/images/x11/dialog_info.png)     |
| Update Panel       | ![](./doc/images/win/panel_checking.png)  | ![](./doc/images/mac/panel_checking.png)  | ![](./doc/images/x11/panel_checking.png)  |
| Update Action      | ![](./doc/images/win/action.png)          | ![](./doc/images/mac/action.png)          | ![](./doc/images/x11/action.png)          |

## Requirements
 - Qt Installer Framework: The updater requires the application to be installed using the framework and will use the frameworks update mechanism to check for updates (https://doc.qt.io/qtinstallerframework/, download at https://download.qt.io/official_releases/qt-installer-framework/)
 - C++11 - The library makes heavy use of it's features
 - Qt 5.5 (the minimum I tested, may work with older versions)
   - If you are using Qt 5.6, the updater will make use of the new QVersionNumber class. If not, it will create a class named the same way that simply inherits QString and adds a few functions.
 - Since the Installer Framework supports Windows, Mac and X11 only, it's the same for this library

## Getting started
The usage of this library is not that complicated. However, to make this work you will have to use the Qt Installer Framework to create and installer/updater. If you already now how to to that, just check out the examples below. If not, you can check out my 
[Tutorial: Create a simple auto-updated application](https://skycoder42.github.io/QtAutoUpdater/tutorial_page.html)

## Examples
**Important:**<br>
Since this library requires the maintenancetool that is deployed with every Qt Installer Framework installation, the examples cannot be tested without a maintenancetool! If you intend to use this library, the maintenancetool will be available for your final application. For testing purpose or the examples, I set the path to the MaintenanceTool that is deployed with the installation of Qt (since you all should have at least that one). So make shure to adjust the path if you try to run the example.

### Updater
The following example shows the basic usage of the updater. It creates a new updater instance that is connected to the maintenancetool located at "./maintenancetool". As soon as the application starts, it will check for updates and print the update result. If updates are available, their details will be printed and the maintenancetool is scheduled to start on exit. In both cases, the application will quit afterwards.

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <updater.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //create the updater with the application as parent -> will live long enough start the tool on exit
    //QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater(&a);
    QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater("C:/Qt/MaintenanceTool", &a);//.exe is automatically added
    
    QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](bool hasUpdate, bool hasError) {
        qDebug() << "Has updates:" << hasUpdate << "\nHas errors:" << hasError;
        if(hasUpdate) {
            //As soon as the application quits, the maintenancetool will be started in update mode
            updater->runUpdaterOnExit();
            qDebug() << "Update info:" << updater->updateInfo();
        }
        //Quit the application
        qApp->quit();
    });
    
    //start the update check
    updater->checkForUpdates();
    return a.exec();
}
```

### UpdateController
This example will show you the full capability of the controller. Since there is no mainwindow in this example, you will only see the controller dialogs. Please not that you can control how much of that dialogset will be shown to the user. This example is *reduced*! for a full example with all parts of the controller, check the Tests/WidegtsTest application.

```cpp
#include <QApplication>
#include <updatecontroller.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Since there is no mainwindow, the various dialogs should not quit the app
    QApplication::setQuitOnLastWindowClosed(false);
    //create the update controller with the application as parent -> will live long enough start the tool on exit
    //since the parent is not a widget, all dialogs will be top-level windows
    //QtAutoUpdater::UpdateController *controller = new QtAutoUpdater::UpdateController(&a);
    QtAutoUpdater::UpdateController *controller = new QtAutoUpdater::UpdateController("C:/Qt/MaintenanceTool", &a);//.exe is automatically added
    
    QObject::connect(updater, &QtAutoUpdater::UpdateController::runningChanged, [updater](bool running) {
        qDebug() << "Running changed:" << running;
        //quit the application as soon as the updating finished
        if(!running)
            qApp->quit();
    });
    
    //start the update check -> AskLevel to give the user maximum control
    controller->start(QtAutoUpdater::UpdateController::AskLevel);
    return a.exec();
}
```

## Documentation
The documentation is available within the releases and on [github pages](https://skycoder42.github.io/QtAutoUpdater/).

The documentation was created using [doxygen](http://www.stack.nl/~dimitri/doxygen/). It includes an HTML-documentation and Qt-Help files that can be included into QtCreator (QtAssistant) to show F1-Help (See [Adding External Documentation](https://doc.qt.io/qtcreator/creator-help.html#adding-external-documentation) for more details).

## Downloads
Downloads are available via [github releases](https://github.com/Skycoder42/QtAutoUpdater/releases). The downloads include:
 - A .pri file for easy project includes (Just add `include(<path_to>/qtautoupdater.pri)` to your project and all the includes/libray imports/... will be done by that file!)
 - Qt5.5.1 precompiled binaries for:
   - Windows
     - msvc2013 x86
     - msvc2013 x64
     - mingw
   - OsX
     - clang x64
   - X11 (Linux)
     - gcc x64
 - The (public) header files needed for these binaries
 - The HTML and QtHelp documentation
 - The (undeployed) widgets example

## Building it yourself
If you want to build the QtAutoUpdater yourself, make shure that you fullfill all the requirements listed above. To build it, there are no other dependencies beside Qt itself. But please note that the project only supports Desktop Windows, OsX and X11. Trying to build it for other configurations will propably fail!

## Icon sources/Links
 - http://www.fatcow.com/free-icons
 - http://www.oxygen-icons.org/
 - http://www.ajaxload.info/
