
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
        qDebug() << "Getting info for track..";

        if ( !album().isNull() )
            connect( album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
        else
        {
            getInfo();
            connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(fetchImage()) );
        }


        return;
    }

    QNetworkReply* r = lastfm::nam()->get(QNetworkRequest(imgUrl));
    connect( r, SIGNAL(finished()), SLOT(onGotImage()));
}


void
ImageTrack::onAlbumGotInfo()
{
    try
    {
        XmlQuery lfm = qobject_cast<QNetworkReply*>(sender())->readAll();

        lastfm::MutableTrack t( *this );
        t.setImageUrl( lastfm::Small, lfm["album"]["image size=small"].text() );
        t.setImageUrl( lastfm::Medium, lfm["album"]["image size=medium"].text() );
        t.setImageUrl( lastfm::Large, lfm["album"]["image size=large"].text() );
        t.setImageUrl( lastfm::ExtraLarge, lfm["album"]["image size=extralarge"].text() );
        t.setImageUrl( lastfm::Mega, lfm["album"]["image size=mega"].text() );

        fetchImage();
    }
    catch (...)
    {
        // we failed to fetch album info so try the track
        getInfo();
        connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(fetchImage()) );
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
