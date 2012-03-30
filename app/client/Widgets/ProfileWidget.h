#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QPointer>
#include <QWidget>

#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/StylableWidget.h"

class ProfileWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class StylableWidget* user;
        class AvatarWidget* avatar;
        class QLabel* name;
        class QLabel* scrobbleCount;
        class QLabel* scrobbles;
        class QLabel* lovedCount;
        class QLabel* loved;

        class ContextLabel* context;

        class StylableWidget* topWeeklyArtists;
        class StylableWidget* topOverallArtists;
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
