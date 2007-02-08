#ifndef QTBROWSER_AL_HELPPLACE_HH
#define QTBROWSER_AL_HELPPLACE_HH

#include "al_widget.hh"
#include "main_window.hh"
extern MainWindow *main_window;

template <typename Widget>
class alHelpPlacePre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alHelpPlacePre(Type type, const QString& id,const QString& parent):
		alWidget(type, id,parent)
	{
	    wnd_ = main_window;
	}

	~alHelpPlacePre() {}
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }	
	virtual QLayout* getViewLayout() { return wnd_->layout(); }	
	void show(bool) {};
};

class alHelpPlace: public alHelpPlacePre<MainWindow_t>
{
public:
	alHelpPlace(const QString& id,const QString& parent):
		alHelpPlacePre<MainWindow_t>(HelpPlace,id,parent)
	{
	}
protected:
	void setAttr(const QString& name,const QString& value);
};

#endif
