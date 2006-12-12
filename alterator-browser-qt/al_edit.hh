#ifndef QTBROWSER_AL_EDIT_HH
#define QTBROWSER_AL_EDIT_HH

#include <QLabel>
#include <QLineEdit>

#include "al_widget.hh"

class AEdit: public QWidget
{
public:
    AEdit(QWidget *parent=0);
    ~AEdit();
    void markRequired(bool);
    void setText(const QString&);
    void setEchoMode(QLineEdit::EchoMode);
    void setReadOnly(bool);
    QString text();
private:
    QLabel *mark;
    QLineEdit *edit;
};

class alEdit: public alWidgetPre<AEdit>
{
public:
	alEdit(const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const;
	void markRequired(bool);
};

#endif
