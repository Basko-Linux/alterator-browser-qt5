
#include "al_textbox.hh"

alTextBox::alTextBox(const QString& id,const QString& parent):
		alWidgetPre<QTextEdit>(TextBox,id,parent)
{
}
