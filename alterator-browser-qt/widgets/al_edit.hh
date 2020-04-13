#ifndef QTBROWSER_AL_EDIT_HH
#define QTBROWSER_AL_EDIT_HH

#include <QLabel>
#include <QLineEdit>

#include "al_widget.hh"

class AEdit: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AEdit(QWidget *parent, const Qt::Orientation);
    ~AEdit();
    void markRequired(bool);
    void setText(const QString&);
    void setEchoMode(QLineEdit::EchoMode);
    void setReadOnly(bool);
    QString text();
    void setAutocompleteRows(const QString&);

Q_SIGNALS:
    void textEdited(const QString&);
    void editingFinished();
    void returnPressed();

private:
    QLabel *mark;
    QLineEdit *edit;
    QHBoxLayout *layout_;
    QAction *m_pw_show;
};

class alEdit: public alWidgetPre<AEdit>
{
public:
	alEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const;
	void markRequired(bool);
};

#endif
