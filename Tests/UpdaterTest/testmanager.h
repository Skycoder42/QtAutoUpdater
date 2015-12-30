#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include <QLinkedList>
#include <QProcess>

class TestManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(OutputFlags outputFlags READ outputFlags WRITE setOutputFlags)

public:
	enum OutputFlag {
		NoOutput = 0x00,
		Info = 0x01,
		ErrorStdout = 0x02,
		AllStderr = 0x04,
		AllStdout = (0x08 | ErrorStdout),

		Default = (Info | ErrorStdout | AllStderr),
		Forwarded = (AllStdout | AllStderr),
		Complete = (Info | AllStdout | AllStderr)
	};
	Q_DECLARE_FLAGS(OutputFlags, OutputFlag)
	Q_FLAG(OutputFlags)

	TestManager(const QString &testProgramPath);

	void addSequentialTest(const QString &testName,
						   const QString &testData = QString());
	void addSequentialTest(const QStringList &arguments);
	void addSequentialTestProgram(const QString &program,
								  const QStringList &arguments = QStringList());

	void addParallelTest(const QString &testName,
						 const QString &testData = QString());
	void addParallelTest(const QStringList &arguments);
	void addParallelTestProgram(const QString &program,
								const QStringList &arguments = QStringList());


	OutputFlags outputFlags() const;
	void setOutputFlags(OutputFlags outputFlags);

public slots:
	int exec();

signals:
	void outputFlagsChanged(OutputFlags flags);

private slots:
	void runNextGroup();

	void processReady(int exitCode, QProcess::ExitStatus exitStatus);
	void processError(QProcess::ProcessError error);

private:
	struct TestInfo {
		QString program;
		QStringList arguments;
		bool isLocalTest;
	};
	typedef QList<TestInfo> TestGroup;

	static QStringList joinArgs(const QString &testName, const QString &testData);
	QString printFormatArgs(TestInfo info);
	void printStdout(TestInfo info, QProcess *process);
	void printStderr(TestInfo info, QProcess *process);
	void printPass(TestInfo info);
	void printFail(TestInfo info, int code, const QString &errorStr = QString());

	const QString &testPath;
	OutputFlags outFlags;

	bool isRunning;
	QLinkedList<TestGroup> testsSequence;
	int passCount;
	int failCount;

	QHash<QProcess *, TestInfo> activeProcesses;
};

#define TESTMANAGER_GUILESS_MAIN_FLAGGED(TestObject, setupFunc, outFlags) \
	int main(int argc, char *argv[]) \
	{ \
		QCoreApplication app(argc, argv); \
		if(app.arguments().size() <= 1) { \
			TestManager manager(QCoreApplication::applicationFilePath()); \
			setupFunc(&manager); \
			return manager.exec(); \
		} else {\
			app.setAttribute(Qt::AA_Use96Dpi, true); \
			TestObject tc; \
			QTEST_SET_MAIN_SOURCE_PATH \
			return QTest::qExec(&tc, argc, argv); \
		} \
	}
#define TESTMANAGER_GUILESS_MAIN(TestObject, setupFunc) TESTMANAGER_GUILESS_MAIN_FLAGGED(TestObject, setupFunc, TestManager::Default)


#define TESTMANAGER_MAIN_FLAGGED(TestObject, setupFunc, outFlags) \
	int main(int argc, char *argv[]) \
	{ \
		QApplication app(argc, argv); \
		if(app.arguments().size() <= 1) { \
			TestManager manager(QCoreApplication::applicationFilePath()); \
			setupFunc(&manager); \
			return manager.exec(); \
		} else {\
			app.setAttribute(Qt::AA_Use96Dpi, true); \
			TestObject tc; \
			QTEST_SET_MAIN_SOURCE_PATH \
			return QTest::qExec(&tc, argc, argv); \
		} \
	}
#define TESTMANAGER_MAIN(TestObject, setupFunc) TESTMANAGER_MAIN_FLAGGED(TestObject, setupFunc, TestManager::Default)

#endif // TESTMANAGER_H
