# QtAutoUpdater
A library (+service) to automatically check for updates and install them. This repository includes:
 - The updater as a library
 - A service that wraps the updater inside of an program

### Requirements:
 - Qt Installer Framework: The updater requires the application to be installed using the framework and will use the frameworks update mechanism to check for updates (https://doc.qt.io/qtinstallerframework/, download at https://download.qt.io/official_releases/qt-installer-framework/)
 - Qt-Solutions: Only neccessary for the service. The service is created using the service-library of Qt-Solutions (https://github.com/qtproject/qt-solutions)
