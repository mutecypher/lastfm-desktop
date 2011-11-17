
#include <QPainter>

#include <lastfm/Track.h>

#include "lib/unicorn/widgets/Label.h"

#include "../Services/ScrobbleService/ScrobbleService.h"
#include "../ActivityListModel.h"

#include "TrackDelegate.h"

TrackDelegate::TrackDelegate( QObject* parent )
    :QStyledItemDelegate( parent )
{
}

void
TrackDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Track track = index.data( ActivityListModel::TrackRole ).value<Track>();
    QDateTime timestamp = index.data( ActivityListModel::TimeStampRole ).toDateTime();
    bool isNowPlaying = timestamp.toTime_t() == 0 || track.sameObject( ScrobbleService::instance().currentTrack() );

    const QImage& image = index.data( Qt::DecorationRole ).value<QImage>();
    QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
    g.setColorAt( 0, isNowPlaying ? QColor( 0xFFFCCA ) : QColor( 0xeeeeee ) );
    g.setColorAt( 1, isNowPlaying ? QColor( 0xeeebb9 ) : QColor( 0xdddddd ) );
    painter->fillRect( option.rect, g );

    painter->setPen(QColor(0xaaaaaa));
    painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());

    painter->drawImage( option.rect.topLeft() + QPoint( 20, 10 ), image );

    QFont f;
    f.setBold( true );
    f.setPointSize( 13 );
    painter->setPen( QColor(0x333333) );
    painter->setFont( f );

    QFontMetrics fm( f );

    painter->setPen( QColor(0x333333) );
    QString trackTitle = fm.elidedText( index.data( ActivityListModel::TrackNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
    painter->drawText( option.rect.left() + 94, option.rect.top() + 23, trackTitle );
    f.setBold( false );
    painter->setFont( f );
    painter->setPen( QColor(0x333333) );
    QString artist = fm.elidedText( index.data( ActivityListModel::ArtistNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
    painter->drawText( option.rect.left() + 94, option.rect.top() + 43, artist );

    painter->setPen( QColor(0x777777) );

    QString timestampString = isNowPlaying ? tr( "Now playing" ) : unicorn::Label::prettyTime( timestamp );

    f.setPointSize( 11 );
    painter->setFont( f );
    painter->drawText( option.rect.left() + 94, option.rect.bottom() - 12, timestampString );
}

