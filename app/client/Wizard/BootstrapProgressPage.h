#ifndef BOOTSTRAPPROGRESSPAGE_H
#define BOOTSTRAPPROGRESSPAGE_H

#include "WizardPage.h"

class BootstrapProgressPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit BootstrapProgressPage();

    void setPluginId( const QString& pluginId );

private:
    void initializePage();
    void cleanupPage();
};

#endif // BOOTSTRAPPROGRESSPAGE_H
