#ifndef IMAGETRACK_H
#define IMAGETRACK_H

#include <QObject>
#include <QImage>

#include <lastfm/Track>

class ImageTrack : public QObject, public lastfm::Track
{
    Q_OBJECT
public:
    ImageTrack( const lastfm::Track& t );
    ImageTrack( const ImageTrack& t );

    ImageTrack& operator=( const ImageTrack& t );
    const QImage& image() const;

public slots:
    void fetchImage();

private slots:
    void onGotImage();

signals:
    void imageUpdated();

private:
    QImage m_image;
};


#endif // IMAGETRACK_H
