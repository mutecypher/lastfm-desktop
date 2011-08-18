#ifndef GEN_PLUGIN_LIST_H_
#define GEN_PLUGIN_LIST_H_

#include <QList>
#include "IPluginInfo.h"

#include "../../../plugins/foobar08/PluginInfo.h"
#include "../../../plugins/foobar09/PluginInfo.h"
#include "../../../plugins/iTunes/PluginInfo.h"
#include "../../../plugins/LFMRadio/PluginInfo.h"
#include "../../../plugins/winamp/PluginInfo.h"
#include "../../../plugins/wmp/PluginInfo.h"
class GenPluginList : public QList<IPluginInfo*>
{
public:
    GenPluginList()
    { 

        *this << (new FooBar08PluginInfo());
        *this << (new FooBar09PluginInfo());
        *this << (new ITunesPluginInfo());
        *this << (new LFMRadioPluginInfo());
        *this << (new WinampPluginInfo());
        *this << (new WmpPluginInfo());

    }

    
    GenPluginList( const GenPluginList& that )
    :QList<IPluginInfo*>()
    {
        *this = that;
    }

    virtual GenPluginList& operator= ( const GenPluginList& that )
    {
        for( QList<IPluginInfo*>::const_iterator i = that.begin(); 
             i != that.end();
             i++ ) {
            *this << (*i)->clone();
        }
        return *this;
    }

    
    virtual ~GenPluginList()
    {
        while( !this->isEmpty()) {
            delete (*this)[0];
            removeAt( 0 );
        }
    }
};

#endif //GEN_PLUGIN_LIST_H_
