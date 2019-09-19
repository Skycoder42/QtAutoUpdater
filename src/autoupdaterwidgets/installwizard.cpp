#include "installwizard_p.h"
#include "ui_componentspage.h"
#include "ui_installpage.h"
#include "ui_successpage.h"
#include "ui_errorpage.h"
#include <QtGui/QPainter>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStyle>
#include <dialogmaster.h>
using namespace QtAutoUpdater;

InstallWizard::InstallWizard(UpdateInstaller *installer, QWidget *parent) :
	QWizard{parent},
	_installer{installer},
	_installPage{new InstallPage{this}},
	_successPage{new SuccessPage{this}},
	_errorPage{new ErrorPage{this}}
{
	_installer->setParent(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	setModal(false);
	setWindowModality(Qt::NonModal);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Updater installer"));
	setOption(QWizard::NoBackButtonOnStartPage);
	setOption(QWizard::NoBackButtonOnLastPage);
	setOption(QWizard::NoCancelButton);

#ifdef Q_OS_LINUX
	setWizardStyle(QWizard::ModernStyle);
#endif

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

void InstallWizard::closeEvent(QCloseEvent *event)
{
	if (currentId() == InstallPageId)
		_installPage->cancel(event);
	else
		event->accept();
}



void QtAutoUpdater::applyHeaderSize(QHeaderView *view, QAbstractItemModel *model)
{
	for (auto i = 0; i < model->columnCount(); ++i) {
		auto sizeHint = model->headerData(i, Qt::Horizontal, Qt::UserRole + 100);
		if (sizeHint.isValid()) {
			auto hintValue = sizeHint.toInt();
			if (hintValue <= 0)
				view->setSectionResizeMode(i, static_cast<QHeaderView::ResizeMode>(-hintValue));
			else {
				view->setSectionResizeMode(i, QHeaderView::Fixed);
				view->resizeSection(i, hintValue);
			}
		}
	}
}



ComponentsPage::ComponentsPage(InstallWizard *parent) :
	QWizardPage{parent},
	_installer{parent->installer()},
	_ui{new Ui::ComponentsPage{}}
{
	_ui->setupUi(this);
	_ui->treeView->setModel(_installer->componentModel());
	applyHeaderSize(_ui->treeView->header(), _ui->treeView->model());
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
	_ui->componentStatusView->setModel(_wizard->installer()->progressModel());
	_ui->componentStatusView->setVisible(false);
	const auto pColumn = _ui->componentStatusView->model()->property("progressColumn");
	if (pColumn.isValid())
		_ui->componentStatusView->setItemDelegateForColumn(pColumn.toInt(), new ProgressDelegate{});
	applyHeaderSize(_ui->componentStatusView->header(), _ui->componentStatusView->model());
	if (!_wizard->installer()->features().testFlag(UpdateInstaller::Feature::DetailedProgress))
		_ui->detailButton->setEnabled(false);

	connect(_wizard->installer(), &UpdateInstaller::updateGlobalProgress,
			this, &InstallPage::updateGlobalProgress);
	connect(_wizard->installer(), &UpdateInstaller::showEula,
			this, &InstallPage::showEula,
			Qt::QueuedConnection); // needed to not break installers that emit eulas from the start method
	connect(_wizard->installer(), &UpdateInstaller::installSucceeded,
			this, &InstallPage::installSucceeded);
	connect(_wizard->installer(), &UpdateInstaller::installFailed,
			this, &InstallPage::installFailed);
}

void InstallPage::cancel(QCloseEvent *event)
{
	if (!_installing && !_installDone) {
		if (event)
			event->accept();
		return;
	}

	if (event)
		event->ignore();
	if (_wizard->installer()->features().testFlag(UpdateInstaller::Feature::CanCancel)) {
		if (DialogMaster::questionT(this,
									tr("Cancel installation"),
									tr("Cancel the update installation? This might leave the application "
									   "in an unrepairable state!"))
			== QMessageBox::Yes) {
			_nextEnabled = false;
			emit completeChanged();
			_wizard->installer()->cancelInstall();
		}
	}
}

InstallPage::~InstallPage() = default;

void InstallPage::initializePage()
{
	_nextText = _wizard->buttonText(QWizard::NextButton);
	_wizard->setButtonText(QWizard::NextButton, tr("Install"));
	_installing = false;
	_installDone = false;
	_hasError = false;
	_nextEnabled = true;
}

void InstallPage::cleanupPage()
{
	_wizard->setButtonText(QWizard::NextButton, _nextText);
}

bool InstallPage::validatePage()
{
	if (_installDone) {
		_wizard->setButtonText(QWizard::NextButton, _nextText);
		return true;
	} else if (!_installing) {
		_installing = true;
		_nextEnabled = _wizard->installer()->features().testFlag(UpdateInstaller::Feature::CanCancel);
		if (_nextEnabled)
			_wizard->setButtonText(QWizard::NextButton, _wizard->buttonText(QWizard::CancelButton));
		else
			_wizard->setButtonText(QWizard::NextButton, _nextText);
		emit completeChanged();
		_wizard->button(QWizard::BackButton)->setEnabled(false);
		_wizard->installer()->startInstall();
		return false;
	} else {
		if (_wizard->installer()->features().testFlag(UpdateInstaller::Feature::CanCancel))
			cancel();
		return false;
	}
}

bool InstallPage::isComplete() const
{
	return _nextEnabled;
}

int InstallPage::nextId() const
{
	return _hasError ? InstallWizard::ErrorPageId : InstallWizard::SuccessPageId;
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

void InstallPage::showEula(const QVariant &id, const QString &htmlText, bool required)
{
	const auto isFirst = _eulaQueue.isEmpty();
	_eulaQueue.enqueue(std::make_tuple(id, htmlText, required));
	if (isFirst)
		showEulaImpl();
}

void InstallPage::installSucceeded(bool shouldRestart)
{
	setField(SuccessPage::ShouldRestartField, shouldRestart);
	_installing = false;
	_installDone = true;
	_nextEnabled = true;
	emit completeChanged();
	_wizard->setButtonText(QWizard::NextButton, _nextText);
	_wizard->button(QWizard::BackButton)->setEnabled(false);
}

void InstallPage::installFailed(const QString &errorMessage)
{
	setField(ErrorPage::ErrorMessageField, errorMessage);
	_installing = false;
	_hasError = true;
	_installDone = true;
	_nextEnabled = true;
	emit completeChanged();
	_wizard->setButtonText(QWizard::NextButton, _nextText);
	_wizard->button(QWizard::BackButton)->setEnabled(false);
}

void InstallPage::showEulaImpl()
{
	const auto &[id, htmlText, required] = _eulaQueue.head();
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

	_eulaQueue.dequeue();
	if (!_eulaQueue.isEmpty())
		showEulaImpl();
}



void InstallPage::ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionProgressBar progressBarOption;
	initStyleOption(&progressBarOption, option, index);
	QApplication::style()->drawControl(QStyle::CE_ProgressBar,
									   &progressBarOption, painter);
}

QSize InstallPage::ProgressDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionProgressBar progressBarOption;
	initStyleOption(&progressBarOption, option, index);
	return QApplication::style()->sizeFromContents(QStyle::CT_ProgressBar, &progressBarOption, option.rect.size());
}

void InstallPage::ProgressDelegate::initStyleOption(QStyleOptionProgressBar *option, const QStyleOptionViewItem &oldOpt, const QModelIndex &index) const
{
	*static_cast<QStyleOption*>(option) = oldOpt;

	const auto progress = index.data().toDouble();
	option->minimum = 0;
	option->maximum = progress < 0.0 ? 0 : 1000;
	option->progress = progress < 0.0 ? -1 : static_cast<int>(progress * 1000);
	option->text = QString::number(static_cast<int>(progress * 100)) + QLatin1Char('%');
	option->textVisible = progress >= 0.0;
}



const QString SuccessPage::ShouldRestartField {QStringLiteral("shouldRestart")};

SuccessPage::SuccessPage(InstallWizard *parent) :
	QWizardPage{parent},
	_installer{parent->installer()},
	_ui{new Ui::SuccessPage{}}
{
	_ui->setupUi(this);
	registerField(ShouldRestartField, _ui->restartCheckBox, "visible");
}

SuccessPage::~SuccessPage() = default;

bool SuccessPage::validatePage()
{
	if (_ui->restartCheckBox->isVisible() &&
		_ui->restartCheckBox->isChecked())
		_installer->restartApplication();
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
	registerField(ErrorMessageField, _ui->errorLabel, "text");
}

ErrorPage::~ErrorPage() = default;

int ErrorPage::nextId() const
{
	return -1;
}
