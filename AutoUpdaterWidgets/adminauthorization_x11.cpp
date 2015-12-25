#include "adminauthorization.h"
#include <unistd.h>
#include <sys/types.h>
#include <QCoreApplication>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <QInputDialog>
#include <QThread>
using namespace QtAutoUpdater;

// has no guarantee to work
bool AdminAuthorization::hasAdminRights()
{
    return getuid() == 0;
}

bool AdminAuthorization::executeAsAdmin(const QString &program, const QStringList &arguments, const QString &workingDirectory)
{
    bool ok = false;
    QString password = QInputDialog::getText(NULL,
                                             QCoreApplication::translate("AdminAuthorization", "Run as root"),
                                             QCoreApplication::translate("AdminAuthorization", "Please enter the root password to run the maintenancetool as root:"),
                                             QLineEdit::Password,
                                             QString(),
                                             &ok,
                                             Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    if(!ok)
        return false;


    QTemporaryFile tFile;
    ((QIODevice*)&tFile)->open(QIODevice::WriteOnly | QIODevice::Text);

    QStringList args;
    args += program;
    args += arguments;
    QTextStream tStream(&tFile);
    tStream << "#!/bin/bash\n"
            << "cd " << workingDirectory << "\n"
            << "echo " << password << " | sudo -S" << qt_create_commandline(args) << " &\n";
    tStream.flush();

    tFile.close();
    tFile.setPermissions(tFile.permissions() | QFileDevice::ExeOther);

    if(QProcess::execute("/bin/bash", {tFile.fileName()}) == 0) {
        return true;
    } else
        return false;
}

