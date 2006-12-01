
#include "updater.hh"
#include "connection.hh"
#include "browser.hh"

Updater::Updater(QObject *parent):
	QObject(parent),
	timer_(new QTimer(this))
{
	timer_->setInterval(500);
	connect(timer_,SIGNAL(timeout()),this,SLOT(doUpdate()));
}

void Updater::start()
{
	timer_->start();
}

void Updater::stop()
{
	timer_->stop();
}

void Updater::doUpdate()
{
	getDocument(getDocParser,"(alterator-request action \"re-get\")");
}


void Updater::doRetry()
{
	getDocument(getDocParser,"(alterator-request action \"re-get\")");
}
