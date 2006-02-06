#include <iostream>
#include <algorithm>

#include <QtGlobal>

#include <layout.hh>

QSize MyBoxLayout::sizeHint() const
{
	if (dirty_) calcGeometry();

	int n = list_.count();
	int m = margin();
	return hint_+((n?(n-1):n)*QSize(spacing(),spacing())) + QSize(2*m,2*m);
}

QSize MyBoxLayout::minimumSize() const
{
	if (dirty_) calcGeometry();
	int n = list_.count();
	int m = margin();
	return minsize_+((n?(n-1):n)*QSize(spacing(),spacing())) + QSize(2*m,2*m);
}

void MyBoxLayout::setChildrenAlign(int align)
{
	alignment_ = align;
}

void MyBoxLayout::addItem( QLayoutItem *item )
{
	if (findWidget(item->widget()) >= 0) return;

	list_.append(new MyLayoutItem(item));
	invalidate();
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
	invalidate();
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

void MyBoxLayout::invalidate()
{
	QLayout::invalidate(); dirty_ = true;
}


//recalculate both minimumHint and minimumSize
void MyBoxLayout::calcGeometry() const
{
	int minw_hint = 0,minw_size = 0;
	int minh_hint = 0,minh_size = 0;
	int n = list_.count();

        for( int i = 0; i<n ;++i ) {
               const MyLayoutItem *o = list_.at(i);

	       if (o->item_->isEmpty()) continue;

		QSize min_hint = o->item_->sizeHint();
		QSize min_size = o->item_->minimumSize();
		
		if (direction_ == horizontal)
		{
			min_hint.transpose();
			min_size.transpose();
		}

		minh_hint += min_hint.height();
		minh_size += min_size.height();
		
		minw_hint = qMax(minw_hint,min_hint.width());
		minw_size = qMax(minw_size,min_size.width());
	}
	
	hint_ = (direction_ == vertical)? QSize(minw_hint,minh_hint) : QSize(minh_hint,minw_hint);
	minsize_ = (direction_ == vertical)? QSize(minw_size,minh_size) : QSize(minh_size,minw_size);
	dirty_ = false;
}



void MyBoxLayout::setGeometry(const QRect& rect)
{
	QLayout::setGeometry(rect);
	
	int count = int(list_.count());
	int m = margin();

	int xpos = rect.x() + m; //start point
	int ypos = rect.y() + m; //start point

	
	//size per one item
	int width = rect.width(); //total available width
	int height = rect.height(); //total available height
	QSize min = minimumSize();

	if (direction_ == horizontal)
	{
		std::swap(width,height);
		min.transpose();
	}
	
	height -=  (count-1)*spacing() + 2*m; //minus spaces
	
	bool height_enough = (height > min.height());

	int rest = height; //value for spacers
	int rcount = 0; //number of spacers
	QVector<QSize> heights(count);//calculated sizes

	//calculate all heights and rest value for spacers
	//calculate rest value for spacers
	for(int i=0;i<count;++i)
	{
		MyLayoutItem *o = list_.at(i);
		if (o->item_->isEmpty())  continue;//skip invisible items

		QSize hint = o->item_->sizeHint();
		QSize min = o->item_->minimumSize();
		QSize wanted = o->size_;

		if (direction_ == horizontal)
		{
			hint.transpose();
			min.transpose();
			wanted.transpose();
		}

		//calculate height,count restes
		int h = 0;
		int a = 0;//spacer detector
		if (!height_enough)
			h = min.height();
		else if (wanted.height() >= 0)
			h = qMax( (height * wanted.height() ) / 100 , min.height() );
		else if (wanted.height() == -1) //default size
			h = hint.height();
		else if (wanted.height() == -2)  //spacer size
			a = 1;

		rest -= h;

		//calculate width
		int w;
		if (wanted.width() >= 0)
			w = (width * wanted.width() ) / 100;
		else
			w = hint.width();

		
		heights[i]=QSize(w,a?-1:h);//height "-1" means spacer
		rcount += a;
	}

	if (rcount) rest = rest/rcount;//divide rest space between spacers

	//now place widgets on layouts
        for(int i=0;i<count;++i)
	{
		MyLayoutItem *o = list_.at(i);
		
		if (o->item_->isEmpty())  continue;

		int w = heights[i].width();
		int h = heights[i].height();
		if (h<0) h=rest;//apply rest space if needed
			
		int x = xpos;
		int align = (o->alignment_ < 0)?alignment_:o->alignment_;//aligment for width
		
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
      invalidate();
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

	invalidate();
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
