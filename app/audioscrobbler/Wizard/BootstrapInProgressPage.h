#ifndef BOOTSTRAP_IN_PROGRESS_PAGE_H
#define BOOTSTRAP_IN_PROGRESS_PAGE_H

#include <QWizardPage>
#include "lib/unicorn/Updater/PluginList.h"
#include <lastfm/Track>

class BootstrapInProgressPage : public QWizardPage {
    Q_OBJECT
public:
    BootstrapInProgressPage( QWizard* p );

    void initializePage();

    virtual bool isComplete() const;

protected slots:
    void startBootstrap();
    void onTrackProcessed( int percentage, const Track& );
    void onPercentageUpload( int percentage );
    void onBootstrapDone( int );
    void onTrackStarted( const Track& );

protected:
    class QProgressBar* m_progressBar;
    class QLabel* m_label;

    bool m_isComplete;
    PluginList m_pluginList;
    class AbstractBootstrapper* m_bootstrapper;
};

#endif //BOOTSTRAP_IN_PROGRESS_PAGE_H

