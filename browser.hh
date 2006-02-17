#ifndef QT_BROWSER_HH
#define QT_BROWSER_HH

#include <QTimer>

#include "sax.hh"

class Updater: public QObject
{
Q_OBJECT
	public:
		Updater();
	public slots:
		void doUpdate();
		void doRetry();
		void start();
		void stop();
	private:
		QTimer *timer_;
};

void getDocParser(alCommand *cmd);

#endif
