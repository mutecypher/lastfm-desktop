#ifndef PLUGINSINSTALLPAGE_H
#define PLUGINSINSTALLPAGE_H

#include "WizardPage.h"

class PluginsInstallPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit PluginsInstallPage();

private:
    void initializePage();
    void cleanupPage();
};

#endif // PLUGINSINSTALLPAGE_H
