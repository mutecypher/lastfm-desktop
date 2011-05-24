/*
 *  PrefWidget.h
 *  PrefPane
 *
 *  Created by Jono on 12/07/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PREF_WIDGET_H_
#define PREF_WIDGET_H_

#include <Cocoa/Cocoa.h>
#include <QtGui/QMacNativeWidget>

class FmLastPrefPanePrefWidget {
public:
    FmLastPrefPanePrefWidget();
    NSView* view();
    QMacNativeWidget* qWidget(){ return widget; }
    void updateGeometry();
    
private:
    QMacNativeWidget* widget;
    class SettingsDialog* dialog;
};

#endif //PREF_WIDGET_H_