#ifndef QTBROWSER_AL_DIALOG_HH
#define QTBROWSER_AL_DIALOG_HH

#include <QDialog>
#include "widgets.hh"

//hack to access protected done() slot
//also ignore escape button press and some other features
class ADialog: public QDialog
{
public:
	ADialog(QWidget *parent = 0);
	QWidget* getView();

protected:
	void closeEvent(QCloseEvent*);
	void keyPressEvent(QKeyEvent*);
	void showEvent(QShowEvent*);
	void paintEvent(QPaintEvent*);

private:
	QWidget *view_vidget;
};


class alDialog: public alWidgetPre<ADialog>
{
public:
	alDialog(const QString& id,const QString& parent,
	         const QString& width,const QString& height):
		alWidgetPre<ADialog>(Dialog,id,parent)
	{
	    qDebug("new dialog");
	    QVBoxLayout *bl = new QVBoxLayout(getViewWidget());
	    bl->setSpacing(5);
	    bl->setMargin(5);
	}
	void setAttr(const QString& name,const QString& value);
	void start() { qDebug("dialog exec");; wnd_->exec(); }
	void stop()  { wnd_->done(0); }
	QWidget* getViewWidget();
	QLayout* getViewLayout();
};

#endif
