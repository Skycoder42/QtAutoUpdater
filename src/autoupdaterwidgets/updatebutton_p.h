#ifndef QTAUTOUPDATER_UPDATEBUTTON_P_H
#define QTAUTOUPDATER_UPDATEBUTTON_P_H

#include "qtautoupdaterwidgets_global.h"
#include "updatebutton.h"

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>

#include <QtGui/QMovie>

#include <QtWidgets/private/qwidget_p.h>

namespace Ui {
class UpdateButton;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT UpdateButtonPrivate : public QWidgetPrivate
{
	Q_DECLARE_PUBLIC(UpdateButton)

public:
	QPointer<Updater> updater;
	QScopedPointer<Ui::UpdateButton> ui;
	QMovie *loadingGif;
	bool showResult = true;

	void _q_changeUpdaterState(Updater::State state);
	void _q_updaterDestroyed();
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_P_H
