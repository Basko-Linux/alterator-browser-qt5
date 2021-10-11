
#include <QKeyEvent>

#include "printfilter.hh"
#include "browser.hh"

PrintFilter::PrintFilter(QObject *parent):
    QObject(parent)
{
}

PrintFilter::~PrintFilter()
{
}

bool PrintFilter::eventFilter(QObject *obj, QEvent *event)
{
     if (event->type() == QEvent::KeyPress) {
         QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
         if( key_event->key() == Qt::Key_Print ) {
            QWidget *w = nullptr;
            QAbstractItemView *v = qobject_cast<QAbstractItemView*>(obj);
            if( v ) {
        	w = v;
            }
            g_browser->takeScreenShot(w);
            return true;
         }
     }
     // standard event processing
     return QObject::eventFilter(obj, event);
}
