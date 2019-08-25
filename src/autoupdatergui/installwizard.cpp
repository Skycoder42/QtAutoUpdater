#include "installwizard_p.h"
#include "ui_componentspage.h"
#include "ui_installpage.h"
#include "ui_successpage.h"
#include "ui_errorpage.h"
#include <dialogmaster.h>
using namespace QtAutoUpdater;

InstallWizard::InstallWizard(UpdateInstaller *installer, QWidget *parent) :
	QWizard{parent},
	_installer{installer},
	_installPage{new InstallPage{this}},
	_successPage{new SuccessPage{this}},
	_errorPage{new ErrorPage{this}}
{
	setModal(false);
	setWindowModality(Qt::NonModal);
	setOption(QWizard::NoBackButtonOnStartPage);
	setOption(QWizard::NoBackButtonOnLastPage);
	setOption(QWizard::NoCancelButtonOnLastPage);

	setPage(InstallPageId, _installPage);
	setPage(SuccessPageId, _successPage);
	setPage(ErrorPageId, _errorPage);
	if (_installer->features().testFlag(UpdateInstaller::Feature::SelectComponents)) {
		_componentsPage = new ComponentsPage{this};
		setPage(ComponentsPageId, _componentsPage);
		setStartId(ComponentsPageId);
	} else
		setStartId(InstallPageId);
}

UpdateInstaller *InstallWizard::installer() const
{
	return _installer;
}



ComponentsPage::ComponentsPage(InstallWizard *parent) :
	QWizardPage{parent},
	_installer{parent->installer()},
	_ui{new Ui::ComponentsPage{}}
{
	_ui->setupUi(this);
	_ui->treeView->setModel(_installer->componentModel());
}

ComponentsPage::~ComponentsPage() = default;

bool ComponentsPage::validatePage()
{
	if (_installer->components().isEmpty()) {
		DialogMaster::warning(this,
							  tr("You must select at least one component to be installed!"),
							  tr("Component selection invalid"));
		return false;
	} else
		return true;
}

int ComponentsPage::nextId() const
{
	return InstallWizard::InstallPageId;
}



InstallPage::InstallPage(InstallWizard *parent) :
	QWizardPage{parent},
	_wizard{parent},
	_ui{new Ui::InstallPage{}}
{
	_ui->setupUi(this);
	_ui->componentStatusView->setVisible(false);
	if (!_wizard->installer()->features().testFlag(UpdateInstaller::Feature::DetailedProgress))
		_ui->detailButton->setEnabled(false);

	connect(_wizard->installer(), &UpdateInstaller::updateGlobalProgress,
			this, &InstallPage::updateGlobalProgress);
	connect(_wizard->installer(), &UpdateInstaller::showEula,
			this, &InstallPage::showEula);
	connect(_wizard->installer(), &UpdateInstaller::installSucceeded,
			this, &InstallPage::installSucceeded);
	connect(_wizard->installer(), &UpdateInstaller::installFailed,
			this, &InstallPage::installFailed);
	connect(_wizard, &InstallWizard::customButtonClicked,
			this, &InstallPage::startInstall);
}

InstallPage::~InstallPage() = default;

void InstallPage::initializePage()
{
	_wizard->setOption(QWizard::HaveCustomButton1, true);
	_wizard->setButtonText(QWizard::CustomButton1, tr("Install"));
	_installDone = false;
	_hasError = false;
}

void InstallPage::cleanupPage()
{
	_wizard->setOption(QWizard::HaveCustomButton1, false);
	_wizard->button(QWizard::CancelButton)->setEnabled(true);
}

bool InstallPage::validatePage()
{
	if (!_installDone)
		return false;

	_wizard->setOption(QWizard::HaveCustomButton1, false);
	return true;
}

bool InstallPage::isComplete() const
{
	return _installDone;
}

int InstallPage::nextId() const
{
	return _hasError ? InstallWizard::ErrorPageId : InstallWizard::SuccessPageId;
}

void InstallPage::startInstall(int btn)
{
	if (btn == QWizard::CustomButton1) {
		_wizard->button(QWizard::CustomButton1)->setEnabled(false);
		_wizard->button(QWizard::CancelButton)->setEnabled(false);
		_wizard->installer()->startInstall();
	}
}

void InstallPage::updateGlobalProgress(double percentage, const QString &status)
{
	if (percentage < 0) {
		if (_ui->progressBar->maximum() != 0) {
			_ui->progressBar->setRange(0, 0);
			_ui->progressBar->setTextVisible(false);
		}
	} else {
		if (_ui->progressBar->maximum() == 0) {
			_ui->progressBar->setRange(0, 1000);
			_ui->progressBar->setTextVisible(true);
		}
		_ui->progressBar->setValue(static_cast<int>(percentage * 1000));
	}

	if (!status.isEmpty())
		_ui->statusLabel->setText(status);
}

void InstallPage::showEula(QUuid id, const QString &htmlText, bool required)
{
	if (required) {
		auto res = DialogMaster::question(this,
										  htmlText,
										  tr("Accept this EULA to continue installation?"),
										  tr("EULA acceptance requested"));
		_wizard->installer()->eulaHandled(id, res == QMessageBox::Yes);
	} else {
		DialogMaster::information(this,
								  htmlText,
								  tr("Installation comes with the following EULA"),
								  tr("EULA information"));
	}
}

void InstallPage::installSucceeded(bool shouldRestart)
{
	setField(SuccessPage::ShouldRestartField, shouldRestart);
	_installDone = true;
	emit completeChanged();
}

void InstallPage::installFailed(const QString &errorMessage)
{
	setField(ErrorPage::ErrorMessageField, errorMessage);
	_hasError = true;
	_installDone = true;
	emit completeChanged();
}



const QString SuccessPage::ShouldRestartField {QStringLiteral("shouldRestart")};

SuccessPage::SuccessPage(InstallWizard *parent) :
	QWizardPage{parent},
	_installer{parent->installer()},
	_ui{new Ui::SuccessPage{}}
{
	_ui->setupUi(this);
	registerField(ShouldRestartField, _ui->restartCheckBox, "enabled");
}

SuccessPage::~SuccessPage() = default;

bool SuccessPage::validatePage()
{
	if (_ui->restartCheckBox->isVisible() &&
		_ui->restartCheckBox->isChecked())
		Q_UNIMPLEMENTED();  // TODO implement
	return true;
}

int SuccessPage::nextId() const
{
	return -1;
}



const QString ErrorPage::ErrorMessageField {QStringLiteral("errorMessage")};

ErrorPage::ErrorPage(InstallWizard *parent) :
	QWizardPage{parent},
	_ui{new Ui::ErrorPage{}}
{
	_ui->setupUi(this);
	registerField(ErrorMessageField, _ui->errorLabel);
}

ErrorPage::~ErrorPage() = default;

int ErrorPage::nextId() const
{
	return -1;
}
