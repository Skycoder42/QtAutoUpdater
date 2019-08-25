#ifndef QTAUTOUPDATER_INSTALLWIZARD_P_H
#define QTAUTOUPDATER_INSTALLWIZARD_P_H

#include <QtCore/QScopedPointer>

#include <QtAutoUpdaterCore/UpdateInstaller>

#include <QtWidgets/QWizard>

#include "qtautoupdatergui_global.h"

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

class Q_AUTOUPDATERGUI_EXPORT InstallWizard : public QWizard
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

private:
	UpdateInstaller *_installer;
	ComponentsPage *_componentsPage = nullptr;
	InstallPage *_installPage;
	SuccessPage *_successPage;
	ErrorPage *_errorPage;
};

class Q_AUTOUPDATERGUI_EXPORT ComponentsPage : public QWizardPage
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

class Q_AUTOUPDATERGUI_EXPORT InstallPage : public QWizardPage
{
	Q_OBJECT

public:
	explicit InstallPage(InstallWizard *parent);
	~InstallPage() override;

	void initializePage() override;
	void cleanupPage() override;
	bool validatePage() override;
	bool isComplete() const override;
	int nextId() const override;

private Q_SLOTS:
	void startInstall(int btn);

	void updateGlobalProgress(double percentage, const QString &status);
	void showEula(QUuid id, const QString &htmlText, bool required);
	void installSucceeded(bool shouldRestart);
	void installFailed(const QString &errorMessage);

private:
	InstallWizard *_wizard;
	QScopedPointer<Ui::InstallPage> _ui;

	bool _installDone = false;
	bool _hasError = false;
};

class Q_AUTOUPDATERGUI_EXPORT SuccessPage : public QWizardPage
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

class Q_AUTOUPDATERGUI_EXPORT ErrorPage : public QWizardPage
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
