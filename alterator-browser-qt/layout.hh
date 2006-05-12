#ifndef QTBROWSER_LAYOUT_HH
#define QTBROWSER_LAYOUT_HH

#include <QList>
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>

struct MyLayoutItem
{
	MyLayoutItem(QWidget *item,QSize size,int alignment):
		item_(new QWidgetItem(item)),
		size_(size),
		alignment_(alignment)
		{}
	~MyLayoutItem() { delete item_; }

	MyLayoutItem(QLayoutItem* item):
		item_(item),
		alignment_(-1)
	 {}

	QLayoutItem *item_; //this item will be removed by Qt System
	QSize	    size_;//wanted size
	int	    alignment_;//wanted size
};

class MyBoxLayout: public QLayout
{
	public:
		enum Direction {horizontal,vertical};
			
		MyBoxLayout(QWidget *parent,Direction dir = vertical):
			QLayout(parent),
			direction_(dir),
			alignment_(-1),
			dirty_(true)
		{
			setSpacing(0);
			setMargin(0);
		}
	
		void invalidate();
		
		//common overloaded methods
		void addItem( QLayoutItem *item );
		QSize sizeHint() const;
		QSize minimumSize() const;
		void setGeometry(const QRect& rect);
		QLayoutItem *itemAt(int index) const;
		QLayoutItem *takeAt(int index);
		int count() const;
		
		//our specific methods
		void addWidget( QWidget *widget ,const QSize& size  = QSize(),int align = -1 );
		void setChildrenAlign(int align);
		void deleteAllItems();

	private:
		 int findWidget(QWidget *w);
		 QList<MyLayoutItem*> list_;
		 Direction direction_;
		 int	alignment_;
		 
		 void calcGeometry() const;
		 //data caching
		 mutable bool	dirty_;
		 mutable QSize	hint_;
		 mutable QSize	minsize_;
};

class MyVBoxLayout: public MyBoxLayout
{
	public:
		MyVBoxLayout(QWidget *parent):
			MyBoxLayout(parent,MyBoxLayout::vertical)
		{}
};

class MyHBoxLayout: public MyBoxLayout
{
	public:
		MyHBoxLayout(QWidget *parent):
			MyBoxLayout(parent,MyBoxLayout::horizontal)
		{}
};

void testLayout();

#endif
