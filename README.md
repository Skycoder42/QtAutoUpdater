# QtAutoUpdater
A library to automatically check for updates and install them. This repository includes:
 - A library with the basic updater (without any GUI) and an update scheduler
 - A second library that requires the first one and adds basic GUI features

## Main Features
##### Core Library
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

##### GUI Library
 - Requires Widgets (no direct QML-support, and I'm not intending to create one. If you want to use the updater in QML, do the connection yourself!)
 - Automated controller to guide the user through the check-for-updates process
   - customizable: you can decide what to show
   - extended information dialog to show basic information about the update
 - "Run as Admin/root" fully supported on all 3 platforms
 - UpdateAction -> a QAction that starts the updater
 - UpdatePanel -> a button with a busy-indicator to to the same

## Requirements
 - Qt Installer Framework: The updater requires the application to be installed using the framework and will use the frameworks update mechanism to check for updates (https://doc.qt.io/qtinstallerframework/, download at https://download.qt.io/official_releases/qt-installer-framework/)
 - C++11 - The library makes heavy use of it's features
 - Qt 5.5 (the minimum I tested, may work with older versions)
   - If you are using Qt 5.6, the updater will make use of the new QVersionNumber class. If not, it will create a class named the same way that simply inherits QString and adds a few functions.
 - Since the Installer Framework supports Windows, Mac and X11 only, it's the same for this library 

## Examples
##### Updater
```c++
#include <QCoreApplication>
#include <QDebug>
#include <updater.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater("<path_to>/maintenancetool", &a);
    updater->runUpdaterOnExit();
    
    QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](bool a, bool b){
        qDebug() << "Has updates:" << a << "\nHas errors:" << b;
        qDebug() << updater->updateInfo();
        qApp->quit();
    });
    
    updater->checkForUpdates();
    return a.exec();
}
```

##### UpdateScheduler
##### UpdateController

## Documentation

## Downloads

## Building it yourself

## Icon sources/Links:
 - http://www.fatcow.com/free-icons
 - http://www.oxygen-icons.org/
 - http://www.ajaxload.info/
