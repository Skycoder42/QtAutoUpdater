/**************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
**
** $QT_END_LICENSE$
**
**************************************************************************/

#include "adminauthorization.h"
#include <unistd.h>
#include <sys/types.h>
#include <QCoreApplication>
#include "messagemaster.h"
using namespace QtAutoUpdater;

// has no guarantee to work
bool AdminAuthorization::hasAdminRights()
{
    return getuid() == 0;
}

bool AdminAuthorization::executeAsAdmin(const QString &, const QStringList &, const QString &)//TODO wokring dir...
{
    MessageMaster::warning(NULL,
                           QCoreApplication::translate("AdminAuthorization", "Error"),
                           QCoreApplication::translate("AdminAuthorization", "Running as root is not supported for Linux systems right now!"));
    return false;
}

