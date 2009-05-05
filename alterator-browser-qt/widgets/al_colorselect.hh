#ifndef QTBROWSER_AL_COLORSELECT_HH
#define QTBROWSER_AL_COLORSELECT_HH

#include <QPushButton>
#include <QLineEdit>

#include "al_widget.hh"

class AColorSelect: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AColorSelect(QWidget *parent, const Qt::Orientation);
    ~AColorSelect();

    void setTitle(const QString&);
    void setSelected(const QString&);
    QString selectedColor();

signals:
	void selected();
private:
    QHBoxLayout *lay;
    QLineEdit *lineedit;
    QPushButton *btn;
    QString dlg_title;

private slots:
    void showDialog();
    void setBtnColor(const QString&);
};

class alColorSelect: public alWidgetPre<AColorSelect>
{
public:
	alColorSelect(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
