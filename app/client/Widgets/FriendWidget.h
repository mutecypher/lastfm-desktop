#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>

#include <lastfm/XmlQuery.h>
#include <lastfm/User.h>
#include <lastfm/Track.h>

#include "lib/unicorn/StylableWidget.h"

class FriendWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class AvatarWidget* avatar;
        class QLabel* name;
        class QLabel* lastTrack;
        class PlayableItemWidget* radio;
        class PlayableItemWidget* multiRadio;
    } ui;

public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0);

    QString name() const;
    QString realname() const;

private:
    void setDetails();

private:
    lastfm::User m_user;
    lastfm::MutableTrack m_recentTrack;
};

#endif // FRIENDWIDGET_H
