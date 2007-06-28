#ifndef QTBROWSER_AL_MAIN_WIDGET_HH
#define QTBROWSER_AL_MAIN_WIDGET_HH

#include <QApplication>

#include "al_widget.hh"
#include "main_window.hh"

extern MainWindow *main_window;

template <typename Widget>
class alMainWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alMainWidgetPre(AlteratorWidgetType type, const QString& id,const QString& parent):
		alWidget(type, id,parent)
	{
	    wnd_ = new Widget(main_window);
	    main_window->setCentralWidget(getViewWidget());
	}

	~alMainWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }	
	virtual QLayout* getViewLayout() { return wnd_->layout(); }	
	void show(bool b) { if(b && wnd_) wnd_->show(); else wnd_->hide(); };
};

class alMainWidget: public alMainWidgetPre<QWidget>
{
public:
	alMainWidget(const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
	void start() {}
	void stop()  { QApplication::closeAllWindows(); }
};

#endif
