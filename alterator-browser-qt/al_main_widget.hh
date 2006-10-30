#ifndef QTBROWSER_AL_MAIN_WIDGET_HH
#define QTBROWSER_AL_MAIN_WIDGET_HH

#include <QApplication>

#include "al_widget.hh"
#include "main_window.hh"

typedef QWidget MainWidget_t;
extern MainWindow *main_window;

template <typename Widget>
class alMainWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alMainWidgetPre(const QString& id,const QString& parent):
		alWidget(id,parent)
	{
		wnd_ = new Widget(main_window);
		wnd_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		main_window->setCentralWidget( wnd_ );
	}

	~alMainWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }	
	virtual QLayout* getViewLayout() { return wnd_->layout(); }	
};

class alMainWidget: public alMainWidgetPre<MainWidget_t>
{
public:
	alMainWidget(const QString& id,const QString& parent):
		alMainWidgetPre<MainWidget_t>(id, parent)
	{
	    new QVBoxLayout(getViewWidget());
	}
	void setAttr(const QString& name,const QString& value);
	void start() { }
	void stop()  { QApplication::closeAllWindows(); }
};

#endif
