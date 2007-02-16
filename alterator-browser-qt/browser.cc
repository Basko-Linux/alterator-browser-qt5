#include <iostream>

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QSplashScreen>
#include <QPointer>
#include <QPainter>

#include "connection.hh"
#include "browser.hh"
#include "main_window.hh"
#include "widgets.hh"
#include "a_pixmaps.hh"
#include "messagebox.hh"
#include "constraints.hh"
#include "mailbox.hh"
#include "utils.hh"


MailBox *mailbox = 0;//mailbox engine
QPointer<QSplashScreen> splash;//single splash screen
MainWindow *main_window = 0;
alWizardFace *wizard_face = 0;
Constraints *constraints = 0;//constraints engine
int emit_locker = 0; //wrong emit protector

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

void newRequest(const QXmlAttributes& attr)
{
	const QString id = attr.value("widget-id");
	const QString type = attr.value("type");
	const QString parent = attr.value("parent");
	Qt::Orientation orientation = Utils::convertOrientation(attr.value("orientation"));
/*
	qDebug("%s: id<%s> type<%s> parent<%s> orientation<%s> sub-type<%s>", __FUNCTION__,
	    qPrintable(id), qPrintable(type), qPrintable(parent),
	    qPrintable(attr.value("orientation")), qPrintable(attr.value("sub-type")) );
*/

	if ("root" == type)
	{
	    const QString subtype = attr.value("sub-type");
	    if ("popup" == subtype) //this is a dialog
	    {
	    	if(parent.isEmpty())
		    new alMainWidget(id,"",orientation);
	    	else
		    new alDialog(id,parent,orientation,attr.value("width"),
		                          attr.value("height"));
	    }
	    else
	    {
		new alVBox(id,parent);
	    }
	}
	else if ("box" == type) new alBox(id,parent,orientation);
	else if ("vbox" == type) new alBox(id,parent,Qt::Vertical);
	else if ("hbox" == type) new alBox(id,parent,Qt::Horizontal);
	else if ("button" == type) new alButton(id,parent);
	else if ("radio" == type) new alRadio(id,parent);
	else if ("label" == type) new alLabel(id,parent);
	else if ("edit" == type) new alEdit(id,parent);
	else if ("textbox" == type) new alTextBox(id,parent);
	else if ("help-place" == type) new alHelpPlace(id,parent);
	else if ("groupbox" == type) new alGroupBox(id,parent,orientation,attr.value("checked"));
	else if ("gridbox" == type) new alGridBox(id,parent,attr.value("columns"));
	else if ("checkbox" == type) new alCheckBox(id,parent);
	else if ("tree" == type) new alTree(id,parent,attr.value("columns"));
	else if ("combobox" == type) new alComboBox(id,parent);
	else if ("tabbox" == type) new alTabBox(id,parent,orientation);
	else if ("tab-page" == type) new alTabPage(id,parent,orientation);
	else if ("progressbar" == type) new alProgressBar(id,parent);
	else if ("slider" == type) new alSlider(id,parent);
	else if ("separator" == type) new alSeparator(id,parent,orientation);
	else if ("spacer" == type) new alSpacer(id,parent);
	else if ("spinbox" == type) new alSpinBox(id,parent);
	else if ("listbox" == type)
	{
	    int cols = attr.value("columns").toInt();
	    if( cols <= 1 )
		new alListBox(id,parent);
	    else
		new alMultiListBox(id,parent,cols);
	}
	else if ("wizardface" == type)
	{
	    if( wizard_face )
		new alBox(id,parent,orientation);
	    else
		wizard_face = new alWizardFace(id,parent,orientation);
	}
	else
	{
	    qDebug("Unknown widget: %s. Make box instead.", qPrintable(type));
	    new alBox(id,parent,orientation);
	}
}

void closeRequest(const QString& id)
{
	if( elements.contains(id) )
	{
	    elements.take(id)->destroyLater();
	}
}

void cleanRequest(const QString& id)
{
	if( !elements.contains(id) )
	    return;

	alWidget *el = elements[id];
	
	QLayout* layout = el->getViewLayout();
	if( layout )
	{
	    for (int i = 0; i < layout->count(); ++i)
		delete layout->takeAt(i);
	}
	
	QList<alWidget*> children = el->findChildren<alWidget*>();
	if( children.size() > 0 )
	{
	    QListIterator<alWidget *> it(children);
	    while( it.hasNext() )
	    {
		alWidget *aw = it.next();
		aw->show(false);
		aw->destroyLater();
	    }
	}
}

void setRequest(const QString& id,const QString& attr,const QString& value)
{
	//qDebug("%s: id<%s> attr<%s> value<%s>", __FUNCTION__, id.toLatin1().data(), attr.toLatin1().data(), value.toLocal8Bit().data());

	++emit_locker;
	if (!elements.contains(id))
	{
	    --emit_locker;
	    return;
	}
	else
	    elements[id]->setAttr(attr,value);
	--emit_locker;
}

void startRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alWidget *aw = elements[id];
	if( aw )
	{
	    if( aw->type() == alWidget::MainWidget  )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m)  m->start();
	    }
	    else if ( aw->type() == alWidget::Dialog  )
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->start();
	    }
	}
}

void stopRequest(const QString& id)
{
	if (!elements.contains(id))
	    return;
	alWidget *aw = elements[id];
	if( aw )
	{
	    if( aw->type() == alWidget::MainWidget  )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m)  m->stop();
	    }
	    else if ( aw->type() == alWidget::Dialog  )
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->stop();
	    }
	}
}


void eventRequest(const QString& id,const QString& value)
{
	if (!elements.contains(id))
	    return;
	else
	    elements[id]->registerEvent(value);
}

void messageboxRequest(const QXmlAttributes& e)
{
    QWidget *parent = QApplication::activeWindow();
/*
    const QString answer =
	AMessageBox::unconvertButton(
	    AMessageBox::selectMessageBox(
		e.value("type"))(parent,
		e.value("title"),
		e.value("message"),
		AMessageBox::convertButtonList(e.value("buttons")),
		AMessageBox::convertButton(e.value("default-button"))
	    )
	);
*/
    AMsgBox msgbox(
	e.value("type"),
	e.value("title"),
	e.value("message"),
	e.value("buttons"),
	parent
	);
    //qDebug("AMsgBox exec");
    const QString answer = AMessageBox::unconvertButton((QMessageBox::StandardButton)msgbox.exec());
    getDocument(getDocParser,answer);
}

void languageChangeRequest(const QString& language)
{
    if( main_window )
	main_window->changeLanguage(language);
}

////////////////////////////////////////////////

void getDocParser(alCommand *cmd)
{
	QXmlAttributes e = cmd->attrs_;
	QString action = e.value("action");

	if ("new" == action)
		newRequest(e);
	else if ("close" == action)
		closeRequest(e.value("widget-id"));
	else if ("clean" == action)
		cleanRequest(e.value("widget-id"));
	else if ("set" == action)
		setRequest(e.value("widget-id"),
			   e.value("name"),
			   cmd->value_);
	else if ("create-event" == action)
		eventRequest(e.value("widget-id"),
			     cmd->value_);
	else if ("splash" == action)
		splashMessage(cmd->value_);
	else if ("start" == action)
		startRequest(e.value("widget-id"));
	else if ("start" == action)
		startRequest(e.value("widget-id"));
	else if ("stop" == action)
		stopRequest(e.value("widget-id"));
	else if ("messagebox" == action)
		messageboxRequest(e);
	else if ("language" == action)
		languageChangeRequest(cmd->value_);
	else if ("retry" == action)
	{
	    if(mailbox)
		QTimer::singleShot(50,mailbox,SLOT(doRetry()));
	}
	else if ("constraints-clear" == action)
	{
	    if(constraints)
		constraints->clear();
	}
	else if ("constraints-apply" == action)
	{
	    if(constraints)
		constraints->apply();
	}
	else if ("constraints-add" == action)
	{
	    if(constraints)
		constraints->add(e.value("name"),
		                 e.value("type"),
			         e.value("params"));
	}
	//qDebug("getDocParser action %s", action.toLatin1().data());
}

////////////////////////////////////////////////

void emitEvent(const QString& id,const QString& type)
{
	if( emit_locker > 0 ) return;

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
}

int main(int argc,char **argv)
{
    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();
    main_window = &mw;
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
