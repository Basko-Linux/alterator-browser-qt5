#include "al_dialog.hh"
#include "hacks.hh"

QDialog2::QDialog2(QWidget *parent):
    QDialog(parent)
{
    setWindowModality(Qt::ApplicationModal);
    QGridLayout *main_layout = new QGridLayout(this);
    if( main_window->haveWindowManager() )
	main_layout->setMargin(5);
    else
	main_layout->setMargin(10);
    view_vidget = new QWidget(this);
    main_layout->addWidget( view_vidget, 0, 0);
}

QWidget* QDialog2::getView()
{
    return view_vidget;
}

void QDialog2::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	int x = QApplication::desktop()->width()/2 - width()/2;
	if( x < 0 ) x = 0;
	int y = QApplication::desktop()->height()/2 - height()/2;
	if( y < 0 ) y = 0;
	move(x, y);
	QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));
    }
}

void QDialog2::paintEvent(QPaintEvent* e)
{
    if( !main_window->haveWindowManager() )
    {
	QDialog::paintEvent(e);
	widget_corners_round(this);
    }
}

void QDialog2::keyPressEvent ( QKeyEvent * e ) 
{
    switch( e->key() )
    {
	case Qt::Key_Escape:
	case Qt::Key_Enter:
	case Qt::Key_Return:
	{
	    e->ignore();
	    break;
	}
	default:
	    QDialog::keyPressEvent(e);
    }
}

void QDialog2::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

// alDialog
QWidget* alDialog::getViewWidget()
{
    return wnd_->getView();
}

QLayout* alDialog::getViewLayout()
{
    return wnd_->getView()->layout();
}

void alDialog::setAttr(const QString& name,const QString& value)
{
	if ("title" == name)
		wnd_->setWindowTitle(value);
	else if ("full-screen" == name)
	{
		if ("true" == value)
			wnd_->showFullScreen();
		else
			wnd_->showNormal();
	}
	else if ("width" == name)
		wnd_->resize(value.toInt(),wnd_->height());
	else if ("height" == name)
		wnd_->resize(wnd_->width(),value.toInt());
	else
		alWidget::setAttr(name,value);
}
