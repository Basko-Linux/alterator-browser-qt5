#ifndef QTBROWSER_AL_DIALOG_HH
#define QTBROWSER_AL_DIALOG_HH

#include <QDialog>
#include "widgets.hh"

//hack to access protected done() slot
//also ignore escape button press and some other features
class QDialog2: public QDialog
{
public:
	QDialog2(QWidget *parent = 0);
	QWidget* getView();

protected:
	void closeEvent(QCloseEvent*);
	void keyPressEvent(QKeyEvent*);
	void showEvent(QShowEvent*);
	void paintEvent(QPaintEvent*);

private:
	QWidget *view_vidget;
};


class alDialog: public alWidgetPre<QDialog2>
{
public:
	alDialog(const QString& id,const QString& parent,
	         const QString& width,const QString& height):
		alWidgetPre<QDialog2>(Dialog,id,parent)
	{
	    wnd_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	    QVBoxLayout *bl = new QVBoxLayout(getViewWidget());
	    bl->setSpacing(5);
	    bl->setMargin(5);
	}
	void setAttr(const QString& name,const QString& value);
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
	QWidget* getViewWidget();
	QLayout* getViewLayout();
};

#endif
