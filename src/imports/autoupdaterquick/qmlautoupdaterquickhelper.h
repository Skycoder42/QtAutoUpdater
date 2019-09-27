#ifndef QMLAUTOUPDATERQUICKHELPER_H
#define QMLAUTOUPDATERQUICKHELPER_H

#include <QtCore/QObject>

#include <QtAutoUpdaterCore/Updater>

namespace QtAutoUpdater {

class QmlAutoUpdaterQuickHelper : public QObject
{
	Q_OBJECT

public:
	explicit QmlAutoUpdaterQuickHelper(QObject *parent = nullptr);

	Q_INVOKABLE void jsOwn(QObject *object) const;
	Q_INVOKABLE void destroy(QObject *object) const;
	Q_INVOKABLE QString secondaryInfoKey(QtAutoUpdater::Updater *updater) const;
	Q_INVOKABLE QString secondaryInfoDisplayName(QtAutoUpdater::Updater *updater) const;
};

}

#endif // QMLAUTOUPDATERQUICKHELPER_H
