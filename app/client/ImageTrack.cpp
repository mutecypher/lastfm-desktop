
#include <QPixmap>

#include <lastfm/ws.h>
#include <lastfm/XmlQuery.h>

#include "ImageTrack.h"


ImageTrack::ImageTrack( const lastfm::Track& t )
    :QObject(), lastfm::Track(t)
{
}

ImageTrack::ImageTrack( const ImageTrack& t )
    :QObject(), lastfm::Track(t)
{
}

ImageTrack&
ImageTrack::operator=( const ImageTrack& t )
{
    d = t.d;
    m_image = t.image();
    return *this;
}

const QImage&
ImageTrack::image() const
{
    return m_image;
}

void
ImageTrack::fetchImage()
{
    const QUrl imgUrl = imageUrl( lastfm::Medium, false );

    if( imgUrl.isEmpty() )
    {
        // We don't know where the image is so first try album.getInfo
        // and then fall back on track.getInfo

        if ( !album().isNull() )
            connect( album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
        else
        {
            getInfo();
            connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(onTrackGotInfo()) );
        }
    }
    else
        connect( lastfm::nam()->get( QNetworkRequest(imgUrl) ), SIGNAL(finished()), SLOT(onGotImage()));
}

void
ImageTrack::onTrackGotInfo()
{
    const QUrl imgUrl = imageUrl( lastfm::Medium, false );

    if( !imgUrl.isEmpty() )
        connect( lastfm::nam()->get( QNetworkRequest(imgUrl) ), SIGNAL(finished()), SLOT(onGotImage()));
}

void
ImageTrack::onAlbumGotInfo()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {

        lastfm::MutableTrack t( *this );
        t.setImageUrl( lastfm::Small, lfm["album"]["image size=small"].text() );
        t.setImageUrl( lastfm::Medium, lfm["album"]["image size=medium"].text() );
        t.setImageUrl( lastfm::Large, lfm["album"]["image size=large"].text() );
        t.setImageUrl( lastfm::ExtraLarge, lfm["album"]["image size=extralarge"].text() );
        t.setImageUrl( lastfm::Mega, lfm["album"]["image size=mega"].text() );

        const QUrl imgUrl = imageUrl( lastfm::Medium, false );

        if ( !imgUrl.isEmpty() )
            connect( lastfm::nam()->get( QNetworkRequest(imgUrl) ), SIGNAL(finished()), SLOT(onGotImage()));
        else
        {
            // We were unable to get the album image from album.getInfo
            // so let Last.fm guess with track.getInfo
            getInfo();
            connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(onTrackGotInfo()) );
        }

    }
}


void
ImageTrack::onGotImage()
{
    qDebug() << "Finished fetching image for track: " << toString();
    sender()->deleteLater();
    QNetworkReply* reply = (QNetworkReply*) sender();
    if (reply && reply->error() == QNetworkReply::NoError)
    {
        QPixmap p;

        p.loadFromData(reply->readAll());
        if (!p.isNull())
        {
            m_image = p.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
        }

        emit imageUpdated();
    }
}
