#ifndef MESSAGEMASTER_H
#define MESSAGEMASTER_H

#include <QMessageBox>
#include <QDebug>

namespace MessageMaster
{
	inline QMessageBox::StandardButton msgBox(QWidget *parent, QMessageBox::Icon icon, const QString &title,
		 const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
		QMessageBox box(icon, title, text, buttons, parent);
		box.setDefaultButton(defaultButton);

		if(parent)
			box.setWindowModality(Qt::WindowModal);
		else
			box.setWindowModality(Qt::ApplicationModal);

		box.setWindowFlags(box.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		return (QMessageBox::StandardButton)box.exec();
	}



	inline QMessageBox::StandardButton information(QWidget *parent, const QString &title,
		 const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
		 QMessageBox::StandardButton defaultButton = QMessageBox::Ok) {
		return msgBox(parent, QMessageBox::Information, title, text, buttons, defaultButton);
	}
	inline QMessageBox::StandardButton question(QWidget *parent, const QString &title,
		 const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
		 QMessageBox::StandardButton defaultButton = QMessageBox::Yes) {
		return msgBox(parent, QMessageBox::Question, title, text, buttons, defaultButton);
	}
	inline QMessageBox::StandardButton warning(QWidget *parent, const QString &title,
		 const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
		 QMessageBox::StandardButton defaultButton = QMessageBox::Ok) {
		return msgBox(parent, QMessageBox::Warning, title, text, buttons, defaultButton);
	}
	inline QMessageBox::StandardButton critical(QWidget *parent, const QString &title,
		 const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
		 QMessageBox::StandardButton defaultButton = QMessageBox::Ok) {
		return msgBox(parent, QMessageBox::Critical, title, text, buttons, defaultButton);
	}
}

#endif // MESSAGEMASTER_H
