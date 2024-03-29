#ifndef QTBROWSER_AL_COMBOBOX_HH
#define QTBROWSER_AL_COMBOBOX_HH

#include <QComboBox>
#include <QKeyEvent>

#include "al_widget.hh"

class AComboBox: public AWidget<QComboBox>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AComboBox(QWidget *parent, const Qt::Orientation);
    ~AComboBox();
Q_SIGNALS:
	void editingFinished();

protected:
	void keyPressEvent(QKeyEvent*);
	void focusOutEvent(QFocusEvent*);
	void focusInEvent(QFocusEvent*);

private:
	bool text_changed_;
private Q_SLOTS:
	void onTextChange(const QString&);
	void onActivate(const QString&);
};

class alComboBox: public alWidgetPre<AComboBox>
{
public:
	alComboBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;

private:
	int counter_;
};

#endif
