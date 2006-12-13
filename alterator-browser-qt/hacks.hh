#ifndef QTBROWSER_HACKS_HH
#define QTBROWSER_HACKS_HH

#include <QDialog>
#include <QListWidget>
#include <QTreeWidget>

#include <QKeyEvent>
#include <QPaintEvent>

#include <QFrame>
#include <QBrush>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

#include "main_window.hh"

extern MainWindow *main_window;

void widget_corners_round(QWidget*);
void fix_wmless_window(QWidget*);

#endif
