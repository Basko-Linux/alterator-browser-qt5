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
    void setSelected(const QString &strcolor, bool user = true);
    QString selectedColor();

signals:
	void selected();
	void changed();
	void editingFinished();

private:
    QHBoxLayout *lay;
    QLineEdit *lineedit;
    QPushButton *btn;
    QString dlg_title;
    QString old_txt;

    void setBtnColor(const QString&);

private slots:
    void showDialog();
    void onUserEdited(const QString&);
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
