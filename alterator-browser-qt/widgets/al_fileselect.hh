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
    void setSelected(const QString&, bool user = true);
    QString selectedFiles();

Q_SIGNALS:
	void selected();
	void changed();
	void editingFinished();

private:
    QHBoxLayout *lay;
    QLineEdit *lineedit;
    QPushButton *btn;
    QString dlg_opts;
    QString dlg_title;
    QString dlg_dir;
    QString dlg_filter;
    QString old_txt;

private Q_SLOTS:
    void showDialog();
    void onUserEdited(const QString&);
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
