#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <functional>

namespace Ui {
	class ProgressDialog;
}

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

signals:
	void canceled();

protected:
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
	Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
