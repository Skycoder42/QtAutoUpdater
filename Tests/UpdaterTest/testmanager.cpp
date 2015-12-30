#include "testmanager.h"
#include <QtTest>
#include <iostream>

TestManager::TestManager(const QString &testProgramPath) :
	testPath(testProgramPath),
	outFlags(Default),
	isRunning(false),
	testsSequence(),
	passCount(0),
	failCount(0),
	activeProcesses()
{
	if(!QMetaType::isRegistered(QMetaType::type("QProcess::ProcessError")))
		qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
	if(!QMetaType::isRegistered(QMetaType::type("QProcess::ExitStatus")))
		qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
}

void TestManager::addSequentialTest(const QString &testName, const QString &testData)
{
	if(this->isRunning)
		return;

	TestGroup group;
	group.append({QString(), joinArgs(testName, testData), true});
	this->testsSequence.append(group);
}

void TestManager::addSequentialTest(const QStringList &arguments)
{
	if(this->isRunning)
		return;

	TestGroup group;
	group.append({QString(), arguments, false});
	this->testsSequence.append(group);
}

void TestManager::addSequentialTestProgram(const QString &program, const QStringList &arguments)
{
	if(this->isRunning)
		return;

	TestGroup group;
	group.append({program, arguments, false});
	this->testsSequence.append(group);
}

void TestManager::addParallelTest(const QString &testName, const QString &testData)
{
	if(this->isRunning)
		return;

	if(this->testsSequence.isEmpty())
		this->testsSequence.append(TestGroup());

	this->testsSequence.last().append({QString(), joinArgs(testName, testData), true});
}

void TestManager::addParallelTest(const QStringList &arguments)
{
	if(this->isRunning)
		return;

	if(this->testsSequence.isEmpty())
		this->testsSequence.append(TestGroup());

	this->testsSequence.last().append({QString(), arguments, false});
}

void TestManager::addParallelTestProgram(const QString &program, const QStringList &arguments)
{
	if(this->isRunning)
		return;

	if(this->testsSequence.isEmpty())
		this->testsSequence.append(TestGroup());

	this->testsSequence.last().append({program, arguments, false});
}

TestManager::OutputFlags TestManager::outputFlags() const
{
	return this->outFlags;
}

void TestManager::setOutputFlags(OutputFlags outputFlags)
{
	if(this->isRunning)
		return;

	this->outFlags = outputFlags;
}

int TestManager::exec()
{
	if(this->isRunning)
		return -1;
	this->isRunning = true;
	this->passCount = 0;
	this->failCount = 0;

	QMetaObject::invokeMethod(this, "runNextGroup", Qt::QueuedConnection);

	QByteArray beginStr = QStringLiteral("********* Start Testmanager \"%1\" *********\n")
						  .arg(this->testPath)
						  .toLocal8Bit();
	std::cout.write(beginStr.constData(), beginStr.size());
	std::cout.flush();

	int res = qApp->exec();

	QByteArray endStr = QStringLiteral("Totals: %1 passed, %2 failed\n********* Finished Testmanager \"%3\" *********\n")
						.arg(this->passCount)
						.arg(this->failCount)
						.arg(this->testPath)
						.toLocal8Bit();
	std::cout.write(endStr.constData(), endStr.size());
	std::cout.flush();

	if(res == 0)
		return ((this->failCount == 0) ? EXIT_SUCCESS : EXIT_FAILURE);
	else
		return res;
}

void TestManager::runNextGroup()
{
	if(this->testsSequence.isEmpty())
		qApp->quit();
	else {
		TestGroup group = this->testsSequence.takeFirst();

		for(TestInfo info : group) {
			QProcess *proc = new QProcess(this);
			proc->setProgram(info.program.isEmpty() ? this->testPath : info.program);
			proc->setArguments(info.arguments);

			connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)),
					this, SLOT(processReady(int,QProcess::ExitStatus)),
					Qt::QueuedConnection);
			connect(proc, SIGNAL(error(QProcess::ProcessError)),
					this, SLOT(processError(QProcess::ProcessError)),
					Qt::QueuedConnection);

			this->activeProcesses.insert(proc, info);
			proc->start(QIODevice::ReadOnly);
		}
	}
}

void TestManager::processReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitStatus != QProcess::NormalExit) {
		this->processError(QProcess::Crashed);
	} else {
		QProcess *process = qobject_cast<QProcess*>(QObject::sender());
		Q_ASSERT(process);

		if(this->activeProcesses.contains(process)) {
			TestInfo info = this->activeProcesses.take(process);

			if(exitCode == EXIT_SUCCESS)
				this->passCount++;
			else
				this->failCount++;

			if(this->outFlags.testFlag(AllStdout) ||
			   ((exitCode != EXIT_SUCCESS) && this->outFlags.testFlag(ErrorStdout)))
				this->printStdout(info, process);
			if(this->outFlags.testFlag(AllStderr))
				this->printStderr(info, process);
			if(this->outFlags.testFlag(Info)) {
				if(exitCode == EXIT_SUCCESS)
					this->printPass(info);
				else
					this->printFail(info, exitCode);
			}

			process->deleteLater();
			if(this->activeProcesses.isEmpty())
				this->runNextGroup();
		}
	}
}

void TestManager::processError(QProcess::ProcessError error)
{
	QProcess *process = qobject_cast<QProcess*>(QObject::sender());
	Q_ASSERT(process);

	switch(error) {
	case QProcess::Crashed:
	case QProcess::FailedToStart:
		if(this->activeProcesses.contains(process)) {
			TestInfo info = this->activeProcesses.take(process);

			this->failCount++;

			if(this->outFlags.testFlag(ErrorStdout))
				this->printStdout(info, process);
			if(this->outFlags.testFlag(AllStderr))
				this->printStderr(info, process);
			if(this->outFlags.testFlag(Info))
				this->printFail(info, error, process->errorString());

			process->deleteLater();
			if(this->activeProcesses.isEmpty())
				this->runNextGroup();
		}
		break;
	default:
		break;
	}
}

QStringList TestManager::joinArgs(const QString &testName, const QString &testData)
{
	QStringList args;
	if(testData.isEmpty())
		args << testName;
	else
		args << (testName + QLatin1Char(':') + testData);
	return args;
}

QString TestManager::printFormatArgs(TestManager::TestInfo info)
{
	if(info.program.isEmpty()) {
		if(info.isLocalTest) {
			QStringList lList = info.arguments.first().split(QLatin1Char(':'));
			QString testName = lList.takeFirst();
			return QStringLiteral("%1(%2)")
					.arg(testName)
					.arg(lList.join(QLatin1Char(':')));
		} else {
			return QStringLiteral("{\"%2\"}")
					.arg(info.arguments.join(QStringLiteral("\", \"")));
		}
	} else if(info.arguments.isEmpty()) {
		return QStringLiteral("\"%1\"")
				.arg(info.program);
	} else {
		return QStringLiteral("\"%1\" with arguments {\"%2\"}")
				.arg(info.program)
				.arg(info.arguments.join(QStringLiteral("\", \"")));
	}
}

void TestManager::printStdout(TestManager::TestInfo info, QProcess *process)
{
	QByteArray sout = process->readAllStandardOutput();
	if(!sout.isEmpty()) {
		QString begin = QStringLiteral("========= STDOUT of %1 =========\n")
						.arg(this->printFormatArgs(info));

		sout = begin.toLocal8Bit() +
			   sout +
			   "\n========= END OF STDOUT =========\n";
		std::cout.write(sout.constData(), sout.size());
		std::cout.flush();
	}
}

void TestManager::printStderr(TestManager::TestInfo info, QProcess *process)
{
	QByteArray serr = process->readAllStandardError();
	if(!serr.isEmpty()) {
		QString begin = QStringLiteral("========= STDERR of %1 =========\n")
						.arg(this->printFormatArgs(info));

		serr = begin.toLocal8Bit() +
			   serr +
			   "\n========= END OF STDERR =========\n";
		std::cerr.write(serr.constData(), serr.size());
		std::cerr.flush();
	}
}

void TestManager::printPass(TestManager::TestInfo info)
{
	QString passString = QStringLiteral("PASS   : %1 - Finished without errors\n")
						 .arg(this->printFormatArgs(info));

	QByteArray sout = passString.toLocal8Bit();
	std::cout.write(sout.constData(), sout.size());
	std::cout.flush();
}

void TestManager::printFail(TestInfo info, int code, const QString &errorStr)
{
	QString failString = QStringLiteral("FAIL!  : %1 - Finished with error code %3\n")
						 .arg(this->printFormatArgs(info))
						 .arg(code);

	if(!errorStr.isEmpty()) {
		failString += QStringLiteral("         Error String:") +
					  errorStr +
					  QLatin1Char('\n');
	}

	QByteArray sout = failString.toLocal8Bit();
	std::cout.write(sout.constData(), sout.size());
	std::cout.flush();
}
