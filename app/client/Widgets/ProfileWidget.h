#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QPointer>
#include <QFrame>

#include <lastfm/Track.h>

#include "lib/unicorn/UnicornSession.h"

namespace unicorn { class Label; }

class ProfileWidget : public QFrame
{
    Q_OBJECT
private:
    struct
    {
        class QFrame* user;
        class AvatarWidget* avatar;
        unicorn::Label* name;
        class QLabel* infoString;
        class QLabel* scrobbleCount;
        class QLabel* scrobbles;
        class QLabel* lovedCount;
        class QLabel* loved;

        class ContextLabel* context;

        class QFrame* topWeeklyArtists;
        class QFrame* topOverallArtists;
    } ui;

public:
    explicit ProfileWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );
    void onGotUserInfo( const lastfm::User& userDetails );

    void onGotTopWeeklyArtists();
    void onGotTopOverallArtists();

    void onGotLibraryArtists();

    void onGotLovedTracks();

    void onScrobblesCached( const QList<lastfm::Track>& tracks );
    void onScrobbleStatusChanged( short scrobbleStatus );
    void setScrobbleCount();

private:
    void changeUser( const QString& newUsername );

private:
    QPointer<QWidget> m_main;

    QString m_currentUsername;

    int m_scrobbleCount;
};

#endif // PROFILEWIDGET_H
