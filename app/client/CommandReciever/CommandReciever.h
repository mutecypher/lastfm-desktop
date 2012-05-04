#ifndef COMMANDRECIEVER_H
#define COMMANDRECIEVER_H

#include <QObject>
#include <QPointer>
#include <QPixmap>

#include <lib/unicorn/TrackImageFetcher.h>
#include <lib/unicorn/UnicornApplicationDelegate.h>

class CommandReciever : public QObject, public unicorn::UnicornApplicationDelegateCommandObserver
{
    Q_OBJECT
public:
    explicit CommandReciever(QObject *parent = 0);
    ~CommandReciever();

    bool artworkDownloaded() const;
    QPixmap getArtwork() const;
    Track track() const;

private:
    QString trackTitle() const;
    QString artist() const;
    QString album() const;
    int duration();
    QPixmap artwork();
    bool loved();

private slots:
    void onFinished( const class QPixmap& image );

    void onTrackSpooled( const Track& track );
    void onStopped();

private:
    QPointer<TrackImageFetcher> m_trackImageFetcher;
    QPixmap m_pixmap;
    bool m_artworkDownloaded;
};

#endif // COMMANDRECIEVER_H
