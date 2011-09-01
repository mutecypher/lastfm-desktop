
#include <QPixmap>

#include <ws/ws.h>

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
ImageTrack::operator=(const ImageTrack& t)
{
    *this = ImageTrack(t);
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
    if( imgUrl.isEmpty())
    {
        qDebug() << "Getting info for track..";
        getInfo();
        connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(fetchImage()), Qt::DirectConnection);
        return;
    }

    QNetworkReply* r = lastfm::nam()->get(QNetworkRequest(imgUrl));
    connect( r, SIGNAL(finished()), SLOT(onGotImage()));
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
