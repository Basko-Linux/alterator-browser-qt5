#include "al_dialog.hh"
#include "hacks.hh"

ADialog::ADialog(QWidget *parent):
    QDialog(0)
{
    //qDebug("new ADialog");
//    setWindowModality(Qt::ApplicationModal);
    QGridLayout *main_layout = new QGridLayout(this);
    if( main_window->haveWindowManager() )
	main_layout->setMargin(1);
    else
	main_layout->setMargin(10);
    view_vidget = new QWidget(this);
//    view_vidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    main_layout->addWidget( view_vidget, 0, 0);
}

QWidget* ADialog::getView()
{
    return view_vidget;
}

void ADialog::showEvent(QShowEvent *e)
{
    //qDebug("ADialog::showEvent");
    //QDialog::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	fix_wmless_window(this);
    }
}

void ADialog::paintEvent(QPaintEvent* e)
{
    if( !main_window->haveWindowManager() )
    {
	//QDialog::paintEvent(e);
	widget_corners_round(this);
    }
}

void ADialog::keyPressEvent ( QKeyEvent * e ) 
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

void ADialog::closeEvent(QCloseEvent *e)
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
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if ("height" == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
	else
		alWidget::setAttr(name,value);
}
