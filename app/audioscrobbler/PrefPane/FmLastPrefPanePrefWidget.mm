/*
 *  PrefWidget.cpp
 *  PrefPane
 *
 *  Created by Jono on 12/07/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "FmLastPrefPanePrefWidget.h"
#include <QApplication>
#include <QMacNativeWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPalette>
#include <QColorDialog>
#include "../../../lib/unicorn/widgets/DataBox.h"

FmLastPrefPanePrefWidget::FmLastPrefPanePrefWidget()
{
    char* argv;
    int argc = 0;
    QApplication* app = new QApplication( argc, &argv, true );
    widget = new QMacNativeWidget();
    QBoxLayout* layout = new QVBoxLayout( widget );
    QLabel* hello = new QLabel( "Testing..", widget);
    QLabel* onetwothree = new QLabel( "1235", widget );
    layout->addWidget( onetwothree );
    layout->addWidget( new DataBox( "Hello", hello, 0 ) );
    layout->addStretch( 1 );
    
    
    foreach( QWidget* w, widget->findChildren<QWidget*>()) {
        w->setAutoFillBackground( true );
        [reinterpret_cast<NSView*>(w->winId()) setAutoresizingMask:NSViewWidthSizable];
    }
    
    widget->layout()->setContentsMargins( 0, 0, 0, 0 );
    widget->layout()->setSpacing( 0 );

    widget->setAutoFillBackground( true );
    QPalette p = widget->palette();
    widget->setPalette( p );
    widget->show();
}

NSView* 
FmLastPrefPanePrefWidget::view()
{
    const WId id = widget->winId();
    const NSView* ret = reinterpret_cast<NSView*>(id);
    return ret;
}