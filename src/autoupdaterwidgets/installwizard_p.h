#ifndef QTAUTOUPDATER_INSTALLWIZARD_P_H
#define QTAUTOUPDATER_INSTALLWIZARD_P_H

#include <QtCore/QScopedPointer>
#include <QtCore/QQueue>

#include <QtAutoUpdaterCore/UpdateInstaller>

#include <QtWidgets/QWizard>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStyledItemDelegate>

#include "qtautoupdaterwidgets_global.h"

namespace Ui {
class ComponentsPage;
class InstallPage;
class SuccessPage;
class ErrorPage;
}

namespace QtAutoUpdater {

class ComponentsPage;
class InstallPage;
class SuccessPage;
class ErrorPage;

class Q_AUTOUPDATERWIDGETS_EXPORT InstallWizard : public QWizard
{
	Q_OBJECT

public:
	enum PageIds {
		ComponentsPageId,
		InstallPageId,
		SuccessPageId,
		ErrorPageId
	};
	Q_ENUM(PageIds)

	explicit InstallWizard(UpdateInstaller *installer, QWidget *parent = nullptr);

	UpdateInstaller *installer() const;

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	UpdateInstaller *_installer;
	ComponentsPage *_componentsPage = nullptr;
	InstallPage *_installPage;
	SuccessPage *_successPage;
	ErrorPage *_errorPage;
};

void Q_AUTOUPDATERWIDGETS_EXPORT applyHeaderSize(QHeaderView *view, QAbstractItemModel *model);

class Q_AUTOUPDATERWIDGETS_EXPORT ComponentsPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit ComponentsPage(InstallWizard *parent);
	~ComponentsPage() override;

	bool validatePage() override;
	int nextId() const override;

private:
	UpdateInstaller *_installer;
	QScopedPointer<Ui::ComponentsPage> _ui;
};

class Q_AUTOUPDATERWIDGETS_EXPORT InstallPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit InstallPage(InstallWizard *parent);
	~InstallPage() override;

	void cancel(QCloseEvent *event = nullptr);

	void initializePage() override;
	void cleanupPage() override;
	bool validatePage() override;
	bool isComplete() const override;
	int nextId() const override;

private Q_SLOTS:
	void updateGlobalProgress(double percentage, const QString &status);
	void showEula(const QVariant &id, const QString &htmlText, bool required);
	void installSucceeded(bool shouldRestart);
	void installFailed(const QString &errorMessage);

private:
	class Q_AUTOUPDATERWIDGETS_EXPORT ProgressDelegate : public QStyledItemDelegate
	{
	public:
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
		QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	private:
		void initStyleOption(QStyleOptionProgressBar *option, const QStyleOptionViewItem &oldOpt, const QModelIndex &index) const;
	};

	InstallWizard *_wizard;
	QScopedPointer<Ui::InstallPage> _ui;
	QString _nextText;

	bool _installing = false;
	bool _installDone = false;
	bool _hasError = false;
	bool _nextEnabled = true;

	QQueue<std::tuple<QVariant, QString, bool>> _eulaQueue;

	void showEulaImpl();
};

class Q_AUTOUPDATERWIDGETS_EXPORT SuccessPage : public QWizardPage
{
	Q_OBJECT

public:
	static const QString ShouldRestartField;

	explicit SuccessPage(InstallWizard *parent);
	~SuccessPage() override;

	bool validatePage() override;
	int nextId() const override;

private:
	UpdateInstaller *_installer;
	QScopedPointer<Ui::SuccessPage> _ui;
};

class Q_AUTOUPDATERWIDGETS_EXPORT ErrorPage : public QWizardPage
{
	Q_OBJECT

public:
	static const QString ErrorMessageField;

	explicit ErrorPage(InstallWizard *parent);
	~ErrorPage() override;

	int nextId() const override;

private:
	QScopedPointer<Ui::ErrorPage> _ui;
};

}

#endif // QTAUTOUPDATER_INSTALLWIZARD_P_H
