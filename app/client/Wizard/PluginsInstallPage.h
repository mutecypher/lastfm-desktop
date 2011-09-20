#ifndef PLUGINSINSTALLPAGE_H
#define PLUGINSINSTALLPAGE_H

#include <QWizardPage>

class PluginsInstallPage : public QWizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit PluginsInstallPage( QWidget* parent = 0 );

private:
    void initializePage();
};

#endif // PLUGINSINSTALLPAGE_H
