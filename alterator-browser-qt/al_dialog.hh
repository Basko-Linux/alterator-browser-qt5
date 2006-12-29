#ifndef QTBROWSER_AL_DIALOG_HH
#define QTBROWSER_AL_DIALOG_HH

#include <QDialog>
#include "widgets.hh"

//hack to access protected done() slot
//also ignore escape button press and some other features
class ADialog: public QDialog
{
    Q_OBJECT
public:
	ADialog(QWidget *parent = 0);
	QWidget* getView();

	void addAction(const QString& key, const QString& name, const QString& pixmap);
	void removeAction(const QString &key);
	void clearActions();
	void setDefaultAction(const QString& key);
	QString currentAction();

protected:
	void closeEvent(QCloseEvent*);
	void keyPressEvent(QKeyEvent*);
	void showEvent(QShowEvent*);
	void paintEvent(QPaintEvent*);

private slots:
	void onButtonClicked(QAbstractButton*);

private:
	QWidget *view_widget;
	QDialogButtonBox *btnbox;
	QString current_action;
	QMap<QString,QDialogButtonBox::StandardButton> key2btn;
	QMap<QString,QAbstractButton*> buttons;
};


class alDialog: public alWidgetPre<ADialog>
{
public:
	alDialog(const QString& id,const QString& parent,
	         const QString& width,const QString& height):
		alWidgetPre<ADialog>(Dialog,id,parent)
	{
	    QVBoxLayout *bl = new QVBoxLayout(getViewWidget());
	    bl->setSpacing(5);
	    bl->setMargin(5);
	}
	void setAttr(const QString& name,const QString& value);
	QString postData() const ;
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
	QWidget* getViewWidget();
	QLayout* getViewLayout();
};

#endif
