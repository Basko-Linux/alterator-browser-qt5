#ifndef QT_BROWSER_PRINTFILTER_HH
#define QT_BROWSER_PRINTFILTER_HH

#include <QObject>

class PrintFilter: public QObject
{
Q_OBJECT
public:
    PrintFilter(QObject *);
    ~PrintFilter();
protected:
    bool eventFilter(QObject *, QEvent *) override;
};

#endif
