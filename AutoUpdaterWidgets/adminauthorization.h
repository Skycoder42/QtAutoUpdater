#ifndef ADMINAUTHORIZATION_H
#define ADMINAUTHORIZATION_H

#include "adminauthoriser.h"

namespace QtAutoUpdater
{
	class AdminAuthorization : public AdminAuthoriser
	{
	public:
		bool hasAdminRights() Q_DECL_OVERRIDE;
		bool executeAsAdmin(const QString &program, const QStringList &arguments, const QString &workingDir) Q_DECL_OVERRIDE;

    private:
        static inline QString qt_create_commandline(const QStringList &arguments);
    };

    QString AdminAuthorization::qt_create_commandline(const QStringList &arguments)
    {
        QString args;
        for (int i = 0; i < arguments.size(); ++i) {
            QString tmp = arguments.at(i);
            // in the case of \" already being in the string the \ must also be escaped
            tmp.replace(QLatin1String("\\\""), QLatin1String("\\\\\""));
            // escape a single " because the arguments will be parsed
            tmp.replace(QLatin1Char('\"'), QLatin1String("\\\""));
            if (tmp.isEmpty() || tmp.contains(QLatin1Char(' ')) || tmp.contains(QLatin1Char('\t'))) {
                // The argument must not end with a \ since this would be interpreted
                // as escaping the quote -- rather put the \ behind the quote: e.g.
                // rather use "foo"\ than "foo\"
                QString endQuote(QLatin1Char('\"'));
                int i = tmp.length();
                while (i > 0 && tmp.at(i - 1) == QLatin1Char('\\')) {
                    --i;
                    endQuote += QLatin1Char('\\');
                }
                args += QLatin1String(" \"") + tmp.left(i) + endQuote;
            } else {
                args += QLatin1Char(' ') + tmp;
            }
        }
        return args;
    }
}

#endif // ADMINAUTHORIZATION_H
