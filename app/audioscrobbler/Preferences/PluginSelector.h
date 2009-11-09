#ifndef PLUGIN_SELECTOR_H
#define PLUGIN_SELECTOR_H

#include "lib/unicorn/stylablewidget.h"
#include "lib/unicorn/UpdateInfoFetcher.h"

#include <QVBoxLayout>
#include <QCheckBox>

class PluginSelector : public StylableWidget
{
    Q_OBJECT
public:
    PluginSelector(void)
    {
        new QVBoxLayout( this );
        QNetworkReply* rep = UpdateInfoFetcher::fetch();
        connect( rep, SIGNAL(finish()), SLOT(onUpdateInfoFetched()));
    }

    ~PluginSelector(void);

private slots:
    void onUpdateInfoFetched()
    {
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        UpdateInfoFetcher info( reply );
        foreach( const Plugin& plugin, info.plugins())
        {
            layout()->addWidget( new QCheckBox( plugin.name(), this ));
        }
    }
};

#endif //PLUGIN_SELECTOR_H