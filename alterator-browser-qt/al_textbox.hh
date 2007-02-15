#ifndef QTBROWSER_AL_TEXTBOX_HH
#define QTBROWSER_AL_TEXTBOX_HH

#include <QTextEdit>
#include <QLabel>

#include "al_widget.hh"

class ATextEdit: public QWidget
{
Q_OBJECT
public:
    ATextEdit(QWidget *parent=0);
    ~ATextEdit();
    void markRequired(bool);
    void setText(const QString&);
    void append(const QString&);
    void setReadOnly(bool);
    QString text();

signals:
    void textEdited();

protected:
    void showEvent(QShowEvent*);

private:
    QLabel *mark;
    QTextEdit *edit;
    QHBoxLayout *layout_;
};

class alTextBox: public alWidgetPre<ATextEdit>
{
public:
	alTextBox(const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};


#endif
