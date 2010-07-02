#ifndef MEDIADEVICE_H
#define MEDIADEVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <lastfm/Track>

class MediaDevice: public QObject
{
    Q_OBJECT
public:
    MediaDevice();
    ~MediaDevice();
    
    QSqlDatabase database() const;

    void setMountPath( const QString& path ){ m_mountPath = path; }

    QString error() const { return m_error; }

    virtual QString tableName() const = 0;
    virtual QList<Track> tracksToScrobble() = 0;

protected:
    QString m_error;
    QString m_mountPath;
};


#endif // MEDIADEVICE_H
