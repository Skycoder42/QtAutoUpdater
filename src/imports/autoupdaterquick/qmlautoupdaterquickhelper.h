#ifndef QMLAUTOUPDATERQUICKHELPER_H
#define QMLAUTOUPDATERQUICKHELPER_H

#include <QtCore/QObject>

class QmlAutoUpdaterQuickHelper : public QObject
{
	Q_OBJECT

public:
	explicit QmlAutoUpdaterQuickHelper(QObject *parent = nullptr);

	Q_INVOKABLE void jsOwn(QObject *object);
	Q_INVOKABLE void destroy(QObject *object);
};

#endif // QMLAUTOUPDATERQUICKHELPER_H
