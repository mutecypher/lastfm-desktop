#ifndef ACTIVITY_LIST_MODEL_H_
#define ACTIVITY_LIST_MODEL_H_

#include <lastfm/ws.h>
#include <lastfm/Track>
#include <QImage>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

namespace unicorn { class Session; }

class ImageTrack : public QObject, public lastfm::Track {
Q_OBJECT
public:
    ImageTrack(const lastfm::Track& t):QObject(), lastfm::Track(t) {}
    ImageTrack(const ImageTrack& t ):QObject(), lastfm::Track(t){}

    ImageTrack& operator=(const ImageTrack& t){ *this = ImageTrack(t); return *this; }
    const QImage& image() const { return m_image; }

public slots:
    void fetchImage() {
        const QUrl imgUrl = imageUrl( lastfm::Small, false );
        if( imgUrl.isEmpty()) {
            qDebug() << "Getting info for track..";
            getInfo();
            connect( signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(fetchImage()), Qt::DirectConnection);
            return;
        }
        QNetworkReply* r = lastfm::nam()->get(QNetworkRequest(imgUrl));
        connect( r, SIGNAL(finished()), SLOT(onGotImage()));
    }


signals:
    void imageUpdated();

private:
    QImage m_image;

private slots:
    void onGotImage() {
        qDebug() << "Finished fetching image for track: " << toString();
        sender()->deleteLater();
        QNetworkReply* reply = (QNetworkReply*) sender();
        if (reply && reply->error() == QNetworkReply::NoError) {
            QPixmap p;

            p.loadFromData(reply->readAll());
            if (!p.isNull()) {
                m_image = p.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
            }
            emit imageUpdated();
        }
    }
};



#include <QAbstractItemModel>
class ActivityListModel : public QAbstractItemModel {
    Q_OBJECT
public:
    enum
    {
        TrackNameRole = Qt::UserRole,
        ArtistNameRole,
        LovedRole,
        TimeStampRole,
        HoverStateRole,
        TrackRole
    };

    ActivityListModel();

    virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex()) const { return createIndex( row, column ); }
    virtual QModelIndex parent( const QModelIndex& index ) const { return QModelIndex(); }
    virtual int rowCount( const QModelIndex& parent = QModelIndex()) const { return parent.isValid() ? 0 : m_tracks.length(); }
    virtual int columnCount( const QModelIndex& parent = QModelIndex()) const { return parent.isValid() ? 0 : 5; }
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags( const QModelIndex& index ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

public slots:
    void onFoundIPodScrobbles( const QList<lastfm::Track>& tracks );
    void onTrackStarted( const Track& );
    void onSessionChanged( unicorn::Session* session );
    void onTrackLoveToggled();

private slots:
    void write() const;

private:
    void read();

private:
    QList<ImageTrack> m_tracks;
    QString m_path;
    QIcon loveIcon;
    QIcon tagIcon;
    QIcon shareIcon;
    QImage noArt;
    QModelIndex hoverIndex;
};

#endif //ACTIVITY_LIST_MODEL_H_
