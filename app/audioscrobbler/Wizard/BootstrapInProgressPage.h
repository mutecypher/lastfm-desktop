#ifndef BOOTSTRAP_IN_PROGRESS_PAGE_H
#define BOOTSTRAP_IN_PROGRESS_PAGE_H

#include <QWizardPage>
#include "lib/unicorn/Updater/PluginList.h"

class BootstrapInProgressPage : public QWizardPage {
public:
    BootstrapInProgressPage( QWizard* p );

    void initializePage();

    virtual bool isComplete() const;

protected:
    bool m_isComplete;
    PluginList m_pluginList;
    class AbstractBootstrapper* m_bootstrapper;
};

#endif //BOOTSTRAP_IN_PROGRESS_PAGE_H

