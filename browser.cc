#include <QApplication>

#include "widgets.hh"
#include "connection.hh"

#include <iostream>


///////////////////////////////

void newRequest(const QString& id, const QString& type, const QString& parent)
{
	if ("vbox" == type) new alVBox(id,parent);
	else if ("hbox" == type) new alHBox(id,parent);
	else if ("dialog" == type) new alDialog(id,parent);
	else if ("button" == type) new alButton(id,parent);
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
	else elements.take(id)->deleteLater();
}

void setRequest(const QString& id,const QString& attr,const QString& value)
{
	if (!elements.contains(id)) return;
	else elements[id]->setAttr(attr,value);
}

void startRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alDialog *d = dynamic_cast<alDialog*>(elements[id]);
	if (d) d->start();
}

void stopRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alDialog *d = dynamic_cast<alDialog*>(elements[id]);
	if (d) d->stop();
}


void eventRequest(const QString& id,const QString& value)
{
	if (!elements.contains(id)) return;
	else elements[id]->registerEvent(value);
}

////////////////////////////////////////////////

void getDocParser(const QDomElement& e)
{
	QString action = e.attribute("action");

	if ("new" == action)
		newRequest(e.attribute("widget-id"),
		           e.attribute("type"),
			   e.attribute("parent"));
	else if ("delete" == action)
		deleteRequest(e.attribute("widget-id"));
	else if ("set" == action)
		setRequest(e.attribute("widget-id"),
			   e.attribute("attr"),
//			   e.attribute("value"));
			   e.text());
	else if ("create-event" == action)
		eventRequest(e.attribute("widget-id"),
//			     e.attribute("value"));
			     e.text());
	else if ("start" == action)
		startRequest(e.attribute("widget-id"));
	else if ("start" == action)
		startRequest(e.attribute("widget-id"));
	else if ("stop" == action)
		stopRequest(e.attribute("widget-id"));
}

////////////////////////////////////////////////

void emitEvent(const QString& id,const QString& type)
{
	std::cerr<<"emitEvent:"<<std::endl;
	QWidget *dlg = QApplication::activeModalWidget();
	if (dlg->accessibleName() == "locked") return;
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

	std::cerr<<"request start ..."<<std::endl;
	getDocument(getDocParser,request);
	std::cerr<<"request end ..."<<std::endl;
	dlg->setAccessibleName("unlocked");
}


int main(int argc,char **argv)
{
        QApplication app(argc, argv);

#if 1
	initConnection();
	getDocument(getDocParser);
#endif



	return 0;
}

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
