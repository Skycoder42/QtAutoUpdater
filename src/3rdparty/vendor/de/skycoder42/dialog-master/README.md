# DialogMaster
A utility to create optimized dialogs with Qt

## Features
Provides a namespace with improved versions of all the dialogs Qt provides. By using these, things like modality, titles and window decoration get handled by the system.
The dialogs include:

- A generic method to setup any QDialog
- QMessageBox, with the common methods as well as a setup for custom message boxes
- QInputDialog
- QProgressDialog
- QFileDialog
- QColorDialog
- QFontDialog

## Installation
The package is providet as qpm package, [`de.skycoder42.dialog-master`](https://www.qpm.io/packages/de.skycoder42.dialog-master/index.html). To install:

1. Install qpm (See [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing))
2. In your projects root directory, run `qpm install de.skycoder42.dialog-master`
3. Include qpm to your project by adding `include(vendor/vendor.pri)` to your `.pro` file

Check their [GitHub - Usage for App Developers](https://github.com/Cutehacks/qpm/blob/master/README.md#usage-for-app-developers) to learn more about qpm.
