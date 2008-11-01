
#include <QApplication>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>

#include "browser.hh"

#include "popup.hh"

Popup::Popup(QWidget *parent, bool title, bool winexpand, bool winclose, bool winshrink):
    QFrame(parent)
{
    has_title = title;
    has_winclose = winclose;
    has_winexpand = winexpand;
    has_winshrink = winshrink;
    QSizePolicy szpol = sizePolicy();

    setAutoFillBackground(true);
    setFrameStyle(QFrame::StyledPanel| QFrame::Raised);

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

    vlayout = new QVBoxLayout(this);
    //vlayout->setMargin(0);
    QHBoxLayout *header_layout = new QHBoxLayout();
    vlayout->addLayout(header_layout);
    header_layout->addWidget(title_txt);
    vlayout->addWidget(title_separator);
    vlayout->addWidget(view_widget);

#if 0
    if( has_winshrink )
    {
	QToolButton *win_shrink = new QToolButton(this);
	win_shrink->setToolButtonStyle(Qt::ToolButtonIconOnly);
	win_shrink->setAutoRaise(true);
	win_shrink->setFixedSize(16,16);
	win_shrink->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarMinButton));
	header_layout->addWidget(win_shrink);
	connect(win_shrink, SIGNAL(clicked()), this, SLOT(onWinShrinkClicked()));
    }

    if( has_winexpand )
    {
	QToolButton *win_expand = new QToolButton(this);
	win_expand->setToolButtonStyle(Qt::ToolButtonIconOnly);
	win_expand->setAutoRaise(true);
	win_expand->setFixedSize(16,16);
	win_expand->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarMaxButton));
	header_layout->addWidget(win_expand);
	connect(win_expand, SIGNAL(clicked()), this, SLOT(onWinExpandClicked()));
    }
#endif

    if( has_winclose )
    {
	QToolButton *win_close = new QToolButton(this);
	win_close->setToolButtonStyle(Qt::ToolButtonIconOnly);
	win_close->setAutoRaise(true);
	win_close->setFixedSize(16,16);
	win_close->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarCloseButton));
	header_layout->addWidget(win_close);
	connect(win_close, SIGNAL(clicked()), this, SLOT(onWinCloseClicked()));
    }
}

Popup::~Popup() {}

void Popup::onWinCloseClicked()
{
    done(QDialogButtonBox::Close);
}

void Popup::onWinExpandClicked()
{
    QSizePolicy szpol_current = sizePolicy();
    QSizePolicy szpol_shrink = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QSizePolicy szpol_expand = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    if( (szpol_current !=  szpol_shrink) && (szpol_current !=  szpol_expand) )
    {
	szpol = szpol_current;
	setSizePolicy(szpol_expand);
    }
    else
	setSizePolicy(szpol);
}

void Popup::onWinShrinkClicked()
{
    QSizePolicy szpol_current = sizePolicy();
    QSizePolicy szpol_shrink = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QSizePolicy szpol_expand = QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    if( (szpol_current !=  szpol_shrink) && (szpol_current !=  szpol_expand) )
    {
	szpol = szpol_current;
	setSizePolicy(szpol_shrink);
	view_widget->setVisible(false);
    }
    else
    {
	setSizePolicy(szpol);
	view_widget->setVisible(true);
    }
}

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