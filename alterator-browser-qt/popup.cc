
#include "browser.hh"

#include "popup.hh"

Popup::Popup(QWidget *parent, bool title, bool winexpand, bool winclose):
    QFrame(parent)
{
    setAutoFillBackground(true);
    setFrameStyle(QFrame::StyledPanel| QFrame::Raised);
    vlayout = new QVBoxLayout(this);
    //vlayout->setMargin(0);
    if( title )
    {
	title_lbl = new QLabel(this);
	title_lbl->setWordWrap(false);
	title_separator = new QFrame(this);
	title_separator->setFrameStyle(QFrame::HLine|QFrame::Sunken);
	vlayout->addWidget(title_lbl);
	vlayout->addWidget(title_separator);
    }
    else
    {
	title_lbl = 0;
	title_separator = 0;
    }
    view_widget = new QWidget(this);
    view_widget->setObjectName("popup_view");
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
	    //e->accept();
	    break;
	}
	case Qt::Key_Escape:
	{
	    //e->accept();
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

void Popup::setPopupTitle(const QString &title)
{
    if( title_lbl )
	title_lbl->setText(title);
}
