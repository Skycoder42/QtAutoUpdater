#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <functional>
#ifdef Q_OS_WIN
#include <QPointer>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QMessageBox>
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
		explicit ProgressDialog(QWidget *parent = 0);
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
		void accept() Q_DECL_OVERRIDE {}
		void reject() Q_DECL_OVERRIDE {}

		void hide(QMessageBox::Icon hideType);

	signals:
		void canceled();

	protected:
#ifdef Q_OS_WIN
		void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
#endif
		void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

	private:
		Ui::ProgressDialog *ui;
#ifdef Q_OS_WIN
		QWinTaskbarButton *tButton;

		void setupTaskbar(QWidget *window);
#endif
	};
}

#endif // PROGRESSDIALOG_H
