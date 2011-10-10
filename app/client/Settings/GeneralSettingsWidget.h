#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include "SettingsWidget.h"

class GeneralSettingsWidget : public SettingsWidget
{
    Q_OBJECT

private:
    struct
    {
        class QComboBox* languages;
    } ui;

public:
    GeneralSettingsWidget( QWidget* parent = 0 );

private:
    void saveSettings();
    void populateLanguages();
};

#endif // GENERALSETTINGSWIDGET_H
