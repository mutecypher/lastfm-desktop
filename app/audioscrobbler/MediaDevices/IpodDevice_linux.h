#ifndef IPODDEVICE_H
#define IPODDEVICE_H

#include "MediaDevice_linux.h"

typedef struct _Itdb_iTunesDB Itdb_iTunesDB;
typedef struct _Itdb_Track Itdb_Track;
typedef struct _Itdb_Playlist Itdb_Playlist;

class IpodDevice: public MediaDevice
{
    Q_OBJECT
public:
    IpodDevice();
    ~IpodDevice();
    virtual QString tableName() const { return m_tableName; }
    virtual QList<Track> tracksToScrobble();
private:
    void commit( Itdb_Track* iTrack );
    void open();
    void setTrackInfo( Track& lstTrack, Itdb_Track* iTrack );
    uint previousPlayCount( Itdb_Track* iTrack ) const;
    QDateTime previousPlayTime( Itdb_Track* track ) const;

private:
    Itdb_iTunesDB* m_itdb;
    Itdb_Playlist* m_mpl;
    QString m_tableName;
    QString m_ipodModel;
};

#endif // IPODDEVICE_H
