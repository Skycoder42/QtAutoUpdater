#include <QtCore>
#include <QtQml>
#include <QtAutoUpdaterCore>
#include <QtQuickTest>
using namespace QtAutoUpdater;

class Setup : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE QVariant createInfoList(const QVariant &info)
	{
		return QVariant::fromValue(QList<UpdateInfo>{info.value<UpdateInfo>()});
	}

public Q_SLOTS:
	void applicationAvailable()
	{
		QCoreApplication::setOrganizationName(QStringLiteral("Skycoder42"));
		QCoreApplication::setOrganizationDomain(QStringLiteral("de.skycoder42"));
		QMetaType::registerConverter<QVersionNumber, QString>([](const QVersionNumber &version) {
			return version.toString();
		});
		QMetaType::registerConverter<QString, QVersionNumber>([](const QString &version) {
			return QVersionNumber::fromString(version);
		});
	}

	void qmlEngineAvailable(QQmlEngine *engine)
	{
		engine->rootContext()->setContextProperty(QStringLiteral("srcDir"), QStringLiteral(SRCDIR));
		engine->rootContext()->setContextProperty(QStringLiteral("setup"), this);
	}
};

QUICK_TEST_MAIN_WITH_SETUP(qmlautoupdatercore, Setup)

#include "tst_qmlautoupdatercore.moc"
