#ifndef ProfileArtistWidget_H
#define ProfileArtistWidget_H

#include <QFrame>

namespace lastfm { class XmlQuery; }

class ProfileArtistWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ProfileArtistWidget( const lastfm::XmlQuery& artist, int maxPlays, QWidget *parent = 0);

};

#endif // ProfileArtistWidget_H
