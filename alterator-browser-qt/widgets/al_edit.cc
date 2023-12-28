#include <QPainter>
#include <QCompleter>
#include <QStringListModel>
#include <QAction>

#include "utils.hh"
#include "al_edit.hh"
#include "a_pixmaps.hh"

AEdit::AEdit(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent),
    m_pw_show(0)
{
    mark = new QLabel(QStringLiteral("*") ,this);
    mark->hide();
    QPalette palet = mark->palette();
    palet.setBrush(QPalette::WindowText, QColor("red"));
    mark->setPalette(palet);

    edit = new QLineEdit(this);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(edit, SIGNAL(textEdited(const QString&)), this, SIGNAL(textEdited(const QString&)));
    connect(edit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    connect(edit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(0,0,0,0);
    layout_->setSpacing(0);
    layout_->addWidget(mark);
    layout_->addWidget(edit);

    setFocusProxy(edit);
}

AEdit::~AEdit() {}

void AEdit::setText(const QString& txt)
{
    edit->setText(txt);
}

void AEdit::setEchoMode(QLineEdit::EchoMode emode)
{
    switch( emode ) {
	case QLineEdit::Password:
	case QLineEdit::PasswordEchoOnEdit:
	    if( !m_pw_show ){
		QAction *m_pw_show = edit->addAction(getPixmap(QStringLiteral("theme:password-show-on")), QLineEdit::TrailingPosition);
		connect(m_pw_show, &QAction::triggered, this, [m_pw_show,this](bool){
		    edit->setEchoMode(edit->echoMode() == QLineEdit::Password ? QLineEdit::Normal: QLineEdit::Password);
		    if( edit->echoMode() == QLineEdit::Password ) {
			m_pw_show->setIcon(getPixmap(QStringLiteral("theme:password-show-on")));
		    } else {
			m_pw_show->setIcon(getPixmap(QStringLiteral("theme:password-show-off")));
		    }
		});
	    }
	    break;
	case QLineEdit::NoEcho:
	    if( m_pw_show ){
		m_pw_show->disconnect();
		m_pw_show->deleteLater();
		m_pw_show = 0;
	    }
	    break;
	case QLineEdit::Normal:
	default:
	    break;
    }
    edit->setEchoMode(emode);
}

void AEdit::setReadOnly(bool ro)
{
    edit->setReadOnly(ro);
}

QString AEdit::text()
{
    return edit->text();
}


void AEdit::markRequired(bool req)
{
    if(req)
    {
	mark->show();
	layout_->setSpacing(5);
    }
    else
    {
	mark->hide();
	layout_->setSpacing(0);
    }
}

void AEdit::setAutocompleteRows(const QString& strrows)
{
    QCompleter *completer = 0;
    if( strrows.split(QLatin1Char(';'), Qt::SkipEmptyParts).size() > 0 )
    {
	QStringList rows = strrows.split(QLatin1Char(';'), Qt::KeepEmptyParts);
	completer = new QCompleter(edit);
	QStringListModel *model = new QStringListModel(rows, completer);
	completer->setModel(model);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer->setCompletionMode(QCompleter::PopupCompletion);
    }
    edit->setCompleter(completer);
}

// alEdit
alEdit::alEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<AEdit>(attr,WEdit,id,parent)
{
}

void alEdit::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("text") == name || QStringLiteral("value") == name)
		wnd_->setText(value);
	else if (QStringLiteral("echo") == name) 
		wnd_->setEchoMode(Utils::convertEchoMode(value));
	else if (QStringLiteral("alterability") == name)
		wnd_->setReadOnly(QStringLiteral("false") == value);
	else if (QStringLiteral("autocompletion-rows") == name)
		wnd_->setAutocompleteRows(value);
	else
		alWidget::setAttr(name,value);
}

void alEdit::registerEvent(const QString& name)
{
	if( QStringLiteral("changed") == name )
		connect(wnd_,SIGNAL( textEdited(const QString&) ),SLOT(onChange(const QString&)));
	else if( QStringLiteral("return-pressed") == name )
	    connect(wnd_,SIGNAL(editingFinished()),SLOT(onReturn()));
}


QString alEdit::postData() const
{
    QString ret;
    ret += QString(QStringLiteral(" (text . \"%1\" )")).arg(Utils::simpleQuote(wnd_->text()));
    ret += QString(QStringLiteral(" (value . \"%1\" )")).arg(Utils::simpleQuote(wnd_->text()));
    return ret;
}

void alEdit::markRequired(bool req)
{
    wnd_->markRequired(req);
}
