#ifndef QTBROWSER_AL_DIALOG_HH
#define QTBROWSER_AL_DIALOG_HH

#include <QScrollArea>

#include "popup.hh"

#include "widgets.hh"

class ADialog: public Popup
{
Q_OBJECT
public:
	ADialog(QWidget *parent, const Qt::Orientation);
	QWidget* getView();

	void addAction(const QString& key, const QString& name, const QString& pixmap);
	void removeAction(const QString &key);
	void clearActions();
	void setDefaultAction(const QString& key);
	QString currentAction();

signals:
	void actionSelected();

protected:
#if 0
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);
	void paintEvent(QPaintEvent*);
	void keyPressEvent(QKeyEvent*);
#endif
	bool eventFilter(QObject*, QEvent*);

private slots:
	void onButtonClicked(QAbstractButton*);

private:
	QScrollArea *scroll;
	QWidget *view_widget;
	QDialogButtonBox *btnbox;
	QString current_action;
	QMap<QString,QDialogButtonBox::StandardButton> key2btn;
	QMap<QString,QAbstractButton*> buttons;
};


class alDialog: public alWidgetPre<ADialog>
{
public:
	alDialog(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	void popUp()
	{
	    connect(wnd_, SIGNAL(finished(int)), browser, SLOT(popupRemoveCurrent(int)));
	    wnd_->exec();
	}
	void popDown()  { wnd_->done(0); }
	QWidget* getViewWidget();
	QLayout* getViewLayout();
};

#endif
