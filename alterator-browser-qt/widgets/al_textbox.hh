#ifndef QTBROWSER_AL_TEXTBOX_HH
#define QTBROWSER_AL_TEXTBOX_HH

#include <QTextBrowser>
#include <QLabel>

#include "al_widget.hh"

class ATextBox: public AWidget<QWidget>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ATextBox(QWidget *parent, const Qt::Orientation);
    ~ATextBox();
    void markRequired(bool);
    void setUrl(const QString&);
    void setText(const QString&);
    void append(const QString&);
    void setReadOnly(bool);
    bool isReadOnly();
    QString text();
    QString url();

signals:
    void textEdited();


protected:
    void showEvent(QShowEvent*);

private slots:
    void execLink(const QUrl&);

private:
    QLabel *mark;
    QTextBrowser *edit;
    QHBoxLayout *layout_;
};

class alTextBox: public alWidgetPre<ATextBox>
{
public:
	alTextBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};


#endif
