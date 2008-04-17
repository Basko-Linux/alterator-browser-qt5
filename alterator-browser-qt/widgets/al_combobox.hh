#ifndef QTBROWSER_AL_COMBOBOX_HH
#define QTBROWSER_AL_COMBOBOX_HH

#include <QComboBox>
#include <QKeyEvent>

#include "al_widget.hh"

class AComboBox: public QComboBox
{
Q_OBJECT
public:
    AComboBox(QWidget *parent, const Qt::Orientation);
    ~AComboBox();
signals:
	void editingFinished();

protected:
	void keyPressEvent(QKeyEvent*);
	void focusOutEvent(QFocusEvent*);
	void focusInEvent(QFocusEvent*);

private:
	bool text_changed_;
private slots:
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
