#ifndef QT_BROWSER_UPDATER_HH
#define QT_BROWSER_UPDATER_HH

#include <QObject>
#include <QTimer>

#include "browser.hh"

class Updater: public QObject
{
Q_OBJECT
	public:
		Updater(QObject *parent);
	public slots:
		void doUpdate();
		void doRetry();
		void start();
		void stop();
	private:
		QTimer *timer_;
};


#endif
