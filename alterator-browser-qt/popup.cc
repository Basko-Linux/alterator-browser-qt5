
//#include <QAbstractButton>
#include <QPushButton>

#include "browser.hh"

#include "popup.hh"

Popup::Popup(QWidget *parent, bool title, bool winexpand, bool winclose):
    QFrame(parent)
{
    has_title = title;
    has_winexpand = winexpand;
    has_winclose = winclose;

    setAutoFillBackground(true);
    setFrameStyle(QFrame::StyledPanel| QFrame::Raised);
    vlayout = new QVBoxLayout(this);
    //vlayout->setMargin(0);
    title_txt = new QLabel(this);
    title_txt->setWordWrap(false);
    QFont title_txt_font = title_txt->font();
    title_txt_font.setBold(true);
    title_txt->setFont(title_txt_font);
    title_separator = new QFrame(this);
    title_separator->setFrameStyle(QFrame::HLine|QFrame::Sunken);
    view_widget = new QWidget(this);
    view_widget->setObjectName("popup_view");

    title_txt->setVisible(has_title);
    title_separator->setVisible(has_title || has_winexpand || has_winclose);

    vlayout->addWidget(title_txt);
    vlayout->addWidget(title_separator);
    vlayout->addWidget(view_widget);
}

Popup::~Popup() {}

void Popup::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
	case Qt::Key_Enter:
	case Qt::Key_Return:
	{
	    QDialogButtonBox *bbox = findChild<QDialogButtonBox*>();
	    if(bbox)
	    {
		bool btn_clicked = false;
		QList<QAbstractButton*> btnlist = bbox->buttons();
		foreach(QAbstractButton* btn, btnlist)
		{
		    if( btn->hasFocus() )
		    {
			btn->click();
			btn_clicked = true;
		    }
		}
		if( !btn_clicked )
		{
		    foreach(QAbstractButton* btn, btnlist)
		    {
			QPushButton *pbtn = qobject_cast<QPushButton*>(btn);
			if( pbtn && pbtn->isDefault() )
			    pbtn->click();
		    }
		}
	    }
	    break;
	}
	case Qt::Key_Escape:
	{
	    if( has_winclose )
		done(QDialogButtonBox::Close);
	    break;
	}
	default:
	    break;
    }
}

QWidget* Popup::view()
{
    return view_widget;
}

void Popup::done(int res)
{
    emit finished(res);
}

int Popup::exec()
{
    browser->popupExec(this);
    return 0;
}

void Popup::setPopupTitle(const QString &txt)
{
    title_txt->setText(txt);
    if( txt.isEmpty() )
	has_title = false;
    else
	has_title = true;
    title_txt->setVisible(has_title);
    title_separator->setVisible(has_title || has_winexpand || has_winclose);
}
