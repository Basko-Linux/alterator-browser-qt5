#include "al_helpplace.hh"

void alHelpPlace::setAttr(const QString& name,const QString& value)
{
    if("source" == name)
	main_window->setHelpSource(value);
}
