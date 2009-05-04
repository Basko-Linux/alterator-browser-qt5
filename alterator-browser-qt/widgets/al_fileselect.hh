#ifndef QTBROWSER_AL_FILESELECT_HH
#define QTBROWSER_AL_FILESELECT_HH

#include <QPushButton>
#include <QLineEdit>

#include "al_widget.hh"

class AFileSelect: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AFileSelect(QWidget *parent, const Qt::Orientation);
    ~AFileSelect();

    void setOptions(const QString&);
    void setTitle(const QString&);
    void setDir(const QString&);
    void setFilter(const QString&);
    QString selectedFiles();

signals:
	void selected();
private:
    QHBoxLayout *lay;
    QLineEdit *lineedit;
    QPushButton *btn;
    QString dlg_opts;
    QString dlg_title;
    QString dlg_dir;
    QString dlg_filter;

private slots:
    void showDialog();
};

class alFileSelect: public alWidgetPre<AFileSelect>
{
public:
	alFileSelect(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
