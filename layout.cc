#include <iostream>
#include <algorithm>

#include <QtGlobal>

#include <layout.hh>

QSize MyBoxLayout::summarize(QSize (QLayoutItem::*mf)() const) const
{
	int minw = 0;
	int minh = 0;
	int n = list_.count();
        for( int i = 0; i<n;++i ) {
               const MyLayoutItem *o = list_.at(i);

	       if (o->item_->isEmpty()) continue;

		QSize min = (o->item_->*mf)();
		if (direction_ == horizontal) min.transpose();

		minh += min.height();
		minw = qMax(minw,min.width());
	}
	return ((direction_ == vertical)? QSize(minw,minh) : QSize(minh,minw))+
		((n?(n-1):n)*QSize(spacing(),spacing()));
}


QSize MyBoxLayout::sizeHint() const
{
	return summarize(&QLayoutItem::sizeHint);
}

QSize MyBoxLayout::minimumSize() const
{
	return summarize(&QLayoutItem::minimumSize);
}

void MyBoxLayout::setChildrenAlign(int align)
{
	alignment_ = align;
}

void MyBoxLayout::addItem( QLayoutItem *item )
{
	if (findWidget(item->widget()) >= 0) return;

	list_.append(new MyLayoutItem(item));
}

void MyBoxLayout::addWidget( QWidget *widget, const QSize& size, int alignment )
{
	int pos;
	if ((pos = findWidget(widget)) >= 0)
	{
		list_.at(pos)->size_ = size;
		list_.at(pos)->alignment_ = alignment;
	}
	else
		list_.append(new MyLayoutItem(widget,size,alignment));
}


int MyBoxLayout::findWidget( QWidget* w )
{
    const int count = list_.count();
    for ( int i = 0; i < count; ++i ) {
       if ( list_.at(i)->item_->widget() == w )
       return i;
    }
    return -1;
}

void MyBoxLayout::setGeometry(const QRect& rect)
{
	int count = int(list_.count());

	int xpos = rect.x(); //start point
	int ypos = rect.y(); //start point

	
	//size per one item
	int width = rect.width(); //total available width
	int height = rect.height(); //total available height
	QSize min = minimumSize();

	if (direction_ == horizontal)
	{
		std::swap(width,height);
		min.transpose();
	}
	
	height -=  (count-1)*spacing(); //minus spaces
	
	bool height_enough = (height > min.height());

        for(int i=0;i<count;++i)
	{
		MyLayoutItem *o = list_.at(i);
		
		if (o->item_->isEmpty())  continue;

		QSize hint = o->item_->sizeHint();
		QSize min = o->item_->minimumSize();
		QSize wanted = o->size_;
		
		if (direction_ == horizontal)
		{
			hint.transpose();
			min.transpose();
			wanted.transpose();
		}
		
		int h;
		if (!height_enough)
			h = min.height();
		else if (wanted.height() >= 0)
			h = qMax( (height * wanted.height() ) / 100 , min.height() );
		else
			h = hint.height();
			
		int w;
		if (wanted.width() >= 0)
			w = (width * wanted.width() ) / 100;
		else
			w = hint.width();

		int x = xpos;
		//aligment for width
		
		int align = (o->alignment_ < 0)?alignment_:o->alignment_;
		
		switch (align)
		{
			case Qt::AlignRight:
				x += (width - w);
				break;
			case Qt::AlignCenter:
				x += ((width - w) / 2);
				break;
			default://Qt:AlignLeft
				break;
		}

		if (direction_ == vertical)
			o->item_->setGeometry(QRect(x,ypos,w,h));
		else
			o->item_->setGeometry(QRect(ypos,x,h,w));

		ypos += h + spacing();
	}
}

void MyBoxLayout::deleteAllItems()
{
      qDeleteAll(list_.begin(), list_.end());
      list_.clear();
//    QLayoutItem *l;
//    while ((l = takeAt(0))) delete l;
}

QLayoutItem *MyBoxLayout::itemAt(int index) const
{
	return index >= 0 && index < list_.count() ? list_.at(index)->item_ : 0;
}

QLayoutItem *MyBoxLayout::takeAt(int index)
{
	if (index >= list_.count()) return 0;
    	MyLayoutItem *b = list_.takeAt(index);
    	QLayoutItem *item = b->item_;
    	b->item_ = 0;
    	delete b;
    	return item;
}

int MyBoxLayout::count() const
{
	return list_.count();
}

#if 0

#include <QLabel>

void testLayout()
{
	QWidget *main = new QWidget();
	
	////// create new layout
	MyVBoxLayout *lay = new MyVBoxLayout(main);
	
	lay->setSpacing(3);
	lay->setMargin(10);

	/////// insert into layout	
	QLabel *b1 = new QLabel("label1",main);
	b1->setFrameStyle( QFrame::Panel | QFrame::Raised );
	lay->addWidget(b1,QSize(70,30),Qt::AlignRight);
	
	QLabel *b2 = new QLabel("label2",main);
	b2->setFrameStyle( QFrame::Panel | QFrame::Raised );
	lay->addWidget(b2,QSize(100,20));
	b2->hide();

	QLabel *b3 = new QLabel("label3",main);
	b3->setFrameStyle( QFrame::Panel | QFrame::Raised );
	lay->addWidget(b3,QSize(30,70),Qt::AlignLeft);

	///// show combound widget
	main->show();
}

#endif
