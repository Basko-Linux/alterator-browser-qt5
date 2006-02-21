#include <iostream>

#include <QApplication>
#include <QSplashScreen>
#include <QPointer>
#include <QPainter>

#include "widgets.hh"
#include "connection.hh"
#include "browser.hh"
#include "main_window.hh"


Updater *updater = 0;//slot for updates
QPointer<QSplashScreen> splash;//single splash screen
MainWindow *main_window = 0;

void splashStart(void)
{
	if (splash) return;

	QPixmap px(250,50);
	px.fill(QApplication::palette().color(QPalette::Background));

	QPainter painter(&px);
	painter.drawLine(1, 2, 1, px.height()-2);
	painter.drawLine(2, 1, px.width()-2,1);
	painter.drawLine(px.width()-1, 2, px.width()-1, px.height()-2);
	painter.drawLine(2, px.height()-1, px.width()-2, px.height()-1);

	splash = new QSplashScreen(px,Qt::WindowStaysOnTopHint);

	splash->show();
}

void splashMessage(const QString& msg)
{
	if (msg.isEmpty())
		delete splash;
	else
	{
		if (!splash) splashStart();
		splash->showMessage(msg);
	}
}


///////////////////////////////

void newRequest(const QString& id, const QString& type, const QString& parent)
{
//	std::cerr<<"NEW:id:"<<id.toUtf8().data()
//	<<",type:"<<type.toUtf8().data()
//	<<",parent:"<<parent.toUtf8().data()
//	<<std::endl;

	if ("dialog" == type)
	{
	    if(parent.isEmpty())
	    {
		new alMainWidget(id,"");
	    }
	    else
	    {
		new alDialog(id,parent);
	    }
	}
	else if ("vbox" == type) new alVBox(id,parent);
	else if ("hbox" == type) new alHBox(id,parent);
	else if ("button" == type) new alButton(id,parent);
	else if ("radio" == type) new alRadio(id,parent);
	else if ("label" == type) new alLabel(id,parent);
	else if ("edit" == type) new alEdit(id,parent);
	else if ("textbox" == type) new alTextBox(id,parent);
	else if ("groupbox" == type) new alGroupBox(id,parent);
	else if ("checkbox" == type) new alCheckBox(id,parent);
	else if ("listbox" == type) new alListBox(id,parent);
	else if ("combobox" == type) new alComboBox(id,parent);
	else if ("tabbox" == type) new alTabBox(id,parent);
	else if ("tab-page" == type) new alTabPage(id,parent);
}

void deleteRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	else
	{
	    alWidget *aw = elements.take(id);
	    aw->getWidget()->hide();
	    aw->deleteLater();
	}
}

void setRequest(const QString& id,const QString& attr,const QString& value)
{
//	std::cerr<<"SET:id:"<<id.toUtf8().data()
//	<<",attr:"<<attr.toUtf8().data()
//	<<",value:"<<value.toUtf8().data()
//	<<std::endl;

	if (!elements.contains(id)) return;
	else elements[id]->setAttr(attr,value);
}

void startRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alWidget *aw = elements[id];
	if( aw )
	    if( aw->stringParent().isEmpty() )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m) m->start();
	    }
	    else
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->start();
	    }
}

void stopRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alWidget *aw = elements[id];
	if( aw )
	    if( aw->stringParent().isEmpty() )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m) m->stop();
	    }
	    else
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->stop();
	    }
}


void eventRequest(const QString& id,const QString& value)
{
	if (!elements.contains(id)) return;
	else elements[id]->registerEvent(value);
}

void timerRequest(const QString& action)
{
	if ("add" == action) updater->start();
	else if ("remove" == action) updater->stop();
}

////////////////////////////////////////////////

void getDocParser(alCommand *cmd)
{
	QXmlAttributes e = cmd->attrs_;
	QString action = e.value("action");

	if ("new" == action)
		newRequest(e.value("widget-id"),
		           e.value("type"),
			   e.value("parent"));
	else if ("delete" == action)
		deleteRequest(e.value("widget-id"));
	else if ("set" == action)
		setRequest(e.value("widget-id"),
			   e.value("attr"),
			   cmd->value_);
	else if ("create-event" == action)
		eventRequest(e.value("widget-id"),
			     cmd->value_);
	else if ("timer" == action)
		timerRequest(cmd->value_);
	else if ("splash" == action)
		splashMessage(cmd->value_);
	else if ("start" == action)
		startRequest(e.value("widget-id"));
	else if ("start" == action)
		startRequest(e.value("widget-id"));
	else if ("stop" == action)
		stopRequest(e.value("widget-id"));
	else if ("retry" == action)
		QTimer::singleShot(50,updater,SLOT(doRetry()));
}

////////////////////////////////////////////////

void emitEvent(const QString& id,const QString& type)
{
	QWidget *dlg = QApplication::activeModalWidget();
	if( !dlg )
	    dlg = main_window;
	if( dlg->accessibleName() == "locked") return;
	dlg->setAccessibleName("locked");

	QString request = "(alterator-request action \"event\"";
	request += "name \""+type+"\"";//append type
	request += "widget-id \""+id+"\"";//append id
	
	//now collect a post data
	request += "\n state (";
	QMapIterator<QString,alWidget*> it(elements);
	while(it.hasNext())
	{
		it.next();
		QString data = it.value()->postData();
		if (!data.isEmpty())
			request += " \n("+it.key() + " "+data+ ")";
	}
	
	request += "))"; //close message

	getDocument(getDocParser,request);
	dlg->setAccessibleName("unlocked");
}

/////////////////////////////////////////////////
Updater::Updater():
	QObject(),
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
	getDocument(getDocParser);
}


int main(int argc,char **argv)
{
        QApplication app(argc, argv);
	MainWindow mw;
	main_window = &mw;
	mw.show();
	int ret = app.exec();
	main_window = 0;

	return ret;
}


#if 0
	QTreeWidget tree;
	tree.setColumnCount(1);
	tree.header()->hide();
	tree.setIndentation(0);

	for(int i=0;i<100;++i)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(&tree);
		item->setText(0,"item");
	}

	QTreeWidgetItem *i = tree.topLevelItem(80);
	tree.setCurrentItem(i);

	tree.show();
	tree.scrollTo(tree.currentIndex());

	app.exec();
#endif


#if 0
	QDialog d;
	d.resize(200,200);

	QFrame2 b1(&d);
	b1.resize(100,100);
	
	d.exec();
#endif



#if 0
	QPixmap px(250,50);
	px.fill(QApplication::palette().color(QPalette::Background));

	QPainter painter(&px);
	painter.drawLine(1, 2, 1, px.height()-2);
	painter.drawLine(2, 1, px.width()-2,1);
	painter.drawLine(px.width()-1, 2, px.width()-1, px.height()-2);
	painter.drawLine(2, px.height()-1, px.width()-2, px.height()-1);

	QSplashScreen *splash = new QSplashScreen(px);

	splash->show();
	splash->showMessage("ZZZZ");
	sleep(2);
	splash->showMessage("DDDD");
	sleep(3);

	delete splash;
#endif




#if 0
	QDialog d;
	d.resize(200,200);

	QTabWidget tabs(&d);
	tabs.resize(100,100);
	QPushButton b1(0);
	QPushButton b2(0);
	b1.setText("a");
	b2.setText("b");
	
	tabs.addTab(&b1,"aaa");
	tabs.addTab(&b2,"aaa");


	d.exec();
#endif


#if 0
	QDialog d;
	d.resize(200,200);
	
	QGroupBox group(&d);
	MyVBoxLayout vbox(&group);
	group.setTitle("zzz");
	group.resize(100,100);
//	group.setLayout(&vbox);
	QPushButton b("uuu",&group);
//	vbox.addWidget(&b);
	vbox.addWidget(&b);
	
	d.exec();
#endif	


#if 0
	//test layout system
	testLayout();
	app.exec();
	//test layout system
#endif

#if 0
	//test dialog
	new alDialog("ddd","");
	elements["ddd"]->setAttr("caption","test");
	//test dialog
	
	//test vbox
	new alVBox("vvv","ddd");
	elements["vvv"]->setAttr("layout-policy","100;100");
	//test vbox
	
	
	//test label
	new alLabel("zzz","vvv");
	elements["zzz"]->setAttr("text","ZZZZ");
	elements["zzz"]->setAttr("layout-policy","50;50");
	//elements["zzz"]->setAttr("pixmap","/usr/share/alterator/images/printers/play.png");
	//test label


	//test button
	new alButton("ggg","vvv");
	elements["ggg"]->setAttr("text","ZZZZ");
	elements["ggg"]->setAttr("layout-policy","50;50;center");
	elements["ggg"]->setAttr("pixmap","/usr/share/alterator/images/printers/play.png");
	//test button

	((alDialog*)(elements["ddd"]))->start();
#endif
