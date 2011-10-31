
#include <QHBoxLayout>

#include <lastfm/XmlQuery.h>
#include <lastfm/Artist.h>
#include <lastfm/RadioStation.h>

#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "ProfileArtistWidget.h"


class PlaysLabel : public QLabel
{
public:
    explicit PlaysLabel( const QString& text, int plays, int maxPlays, QWidget* parent = 0 )
        :QLabel( text, parent ),
          m_plays( plays ),
          m_maxPlays( maxPlays )
    {}

private:
    void paintEvent( QPaintEvent* event )
    {
        QPainter p;
        p.begin( this );

        QFontMetrics fm( font() );
        int indent = fm.width( tr( "%L1 plays" ).arg( 999999 ) );
        int chunk = ( (width() - indent ) * m_plays ) / m_maxPlays;
        int adjust = indent + chunk - width();
        p.setPen( QColor( 0x2a8bad ) );
        p.setBrush( QColor( 0x34bae8 ) );
        p.drawRoundedRect( rect().adjusted( 0, 0, adjust, 0 ), 4, 4 );

        p.end();

        QLabel::paintEvent( event );
    }

private:
    int m_plays;
    int m_maxPlays;
};


ProfileArtistWidget::ProfileArtistWidget( const lastfm::XmlQuery& artist, int maxPlays, QWidget* parent)
    :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    HttpImageWidget* artistImage = new HttpImageWidget( this );
    layout->addWidget( artistImage );
    artistImage->setObjectName( "artistImage" );

    QRegExp re( "/serve/(\\d*)s?/" );

    artistImage->loadUrl( artist["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
    artistImage->setHref( artist["url"].text() );

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins( 0, 0, 0, 0 );
    vl->setSpacing( 0 );
    layout->addLayout( vl, 1 );

    QLabel* artistName = new QLabel( artist["name"].text(), this );
    vl->addWidget( artistName );
    artistName->setObjectName( "artistName" );

    int playcount = artist["playcount"].text().toInt();
    PlaysLabel* plays = new PlaysLabel( tr( playcount == 1 ? "%L1 play" : "%L1 plays" ).arg( playcount ), playcount, maxPlays, this );
    vl->addWidget( plays );
    plays->setObjectName( "plays" );

    PlayableItemWidget* radio = new PlayableItemWidget( RadioStation::similar( Artist( artist["name"].text() ) ), tr( "%1 Radio" ).arg( artist["name"].text() ) );
    vl->addWidget( radio );
    radio->setObjectName( "radio" );
}

