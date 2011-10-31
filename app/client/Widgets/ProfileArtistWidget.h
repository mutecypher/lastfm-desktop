#ifndef ProfileArtistWidget_H
#define ProfileArtistWidget_H

#include <QWidget>

#include "lib/unicorn/StylableWidget.h"

namespace lastfm { class XmlQuery; }

class ProfileArtistWidget : public StylableWidget
{
    Q_OBJECT
public:
    explicit ProfileArtistWidget( const lastfm::XmlQuery& artist, int maxPlays, QWidget *parent = 0);

};

#endif // ProfileArtistWidget_H
