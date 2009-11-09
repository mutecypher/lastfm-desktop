#ifndef BOOTSTRAP_WIZARD_H
#define BOOTSTRAP_WIZARD_H

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QNetworkReply>

#include "lib/unicorn/stylablewidget.h"
#include "lib/unicorn/UpdateInfoFetcher.h"

class SelectBSMediaPlayer: public StylableWidget
{
    Q_OBJECT
public:
    SelectBSMediaPlayer( QWidget* parent = 0 )
    :StylableWidget( parent) { 
        QNetworkReply* reply = UpdateInfoFetcher::fetchInfo();
        connect( reply, SIGNAL( finished()), SLOT(updateInfoFetched()));
        new QVBoxLayout( this );
    }

private slots:
    void updateInfoFetched() {
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        UpdateInfoFetcher info( reply, this );
        QLabel* label = new QLabel( tr( "We can import your listening history from the following media players:"));
        label->setWordWrap( true );
        layout()->addWidget(label);
        foreach( const Plugin& plugin, info.plugins()) {
            if( plugin.isInstalled() && 
                plugin.isPluginInstalled() &&
                plugin.canBootstrap())
                layout()->addWidget( new QRadioButton( plugin.name(), this ));
        }
        layout()->addWidget( new QRadioButton( tr("None"), this ));
        ((QBoxLayout*)layout())->addStretch();
        update();
    }
};

class BootstrapWizard: public StylableWidget
{
public:
    BootstrapWizard( QWidget* parent = 0 )
    :StylableWidget( parent ) {
        new QVBoxLayout( this );
        layout()->addWidget( new SelectBSMediaPlayer( this ));
    }
    ~BootstrapWizard();
};

#endif //BOOTSTRAP_WIZARD_H