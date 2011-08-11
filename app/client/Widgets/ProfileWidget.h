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

        class StylableWidget* topWeeklyArtists;
        class StylableWidget* topOverallArtists;
    } ui;

public:
    explicit ProfileWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );
    void onGotUserInfo( const lastfm::UserDetails& userDetails );

    void onGotTopWeeklyArtists();
    void onGotTopOverallArtists();

private:
    QPointer<QWidget> m_main;
};

#endif // PROFILEWIDGET_H
