
#include "popup.hh"

Popup::Popup(QWidget *parent):
    QDialog(parent)
{
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
