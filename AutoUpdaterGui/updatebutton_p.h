#ifndef UPDATEBUTTON_P_H
#define UPDATEBUTTON_P_H

#include <QPointer>
#include <QMovie>
#include <QScopedPointer>
#include "updatebutton.h"

namespace Ui {
	class UpdateButton;
}

namespace QtAutoUpdater
{
	class UpdateButtonPrivate
	{
	public:
		UpdateButton *q_ptr;
		Q_DECLARE_PUBLIC(UpdateButton)

		QPointer<UpdateController> controller;
		QScopedPointer<Ui::UpdateButton> ui;
		UpdateController::DisplayLevel level;
		QMovie *loadingGif;
		bool showResult;

		UpdateButtonPrivate(UpdateButton *q_ptr, UpdateController *controller);
		~UpdateButtonPrivate();

		void updateController(UpdateController *controller);
	};
}

#endif // UPDATEBUTTON_P_H
