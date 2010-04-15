#include <QtGui>
#include "al_checktree.hh"

int main( int argc, char *argv[] ) {

    // Application instance
    QApplication app( argc, argv );
    // Main widget
    QDialog *dialog = new QDialog();

    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    ACheckTree *w = new ACheckTree( (QWidget *)dialog );
        
    w->addRow( QStringList(QString("true;Leaf 1;;group_1").split(";")) );
    w->addRow( QStringList(QString("true;Group_1;group_1;").split(";")) );
    w->addRow( QStringList(QString("false;Group_2").split(";")) );
    w->addRow( QStringList(QString("false;Group_3;group_2;;false").split(";")) );
    w->addRow( QStringList(QString("false;Leaf 2;;group_2").split(";")) );
    
    mainLayout->addWidget( w );
    dialog->setLayout( mainLayout );
    
    // Run dialog and process files
    dialog->show();
    if( dialog->exec() == 1 ) {
	exit( 1 );
    }
    return 0;
}
