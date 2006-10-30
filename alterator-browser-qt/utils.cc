
#include "utils.hh"


namespace Utils
{

QString reparentTag(QString parent)
{
       return parent.replace(QString(":reparent:"),QString(""));
}

}
