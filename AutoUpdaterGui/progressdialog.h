#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <functional>
#include <QMessageBox>
#include <QScopedPointer>
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#endif

namespace Ui {
	class ProgressDialog;
}

namespace QtAutoUpdater
{
	class ProgressDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit ProgressDialog(QWidget *parent = nullptr);
		~ProgressDialog();

		template <class Class>
		void open(Class *object, void(Class::* member)(int,bool)) {
			connect(this, &ProgressDialog::canceled, object, [=](){
				(object->*member)(3000, true);
			});
			this->show();
		}

		void setCanceled();

	public slots:
		void accept() override {}
		void reject() override {}

		void hide(QMessageBox::Icon hideType);

	signals:
		void canceled();

	protected:
#ifdef Q_OS_WIN
		void showEvent(QShowEvent *event) override;
#endif
		void closeEvent(QCloseEvent *event) override;

	private:
		QScopedPointer<Ui::ProgressDialog> ui;
#ifdef Q_OS_WIN
		QWinTaskbarButton *tButton;

		void setupTaskbar(QWidget *window);
#endif
	};
}

#endif // PROGRESSDIALOG_H
