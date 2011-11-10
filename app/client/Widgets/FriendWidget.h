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
        class Label* name;
        class Label* lastTrack;
        class PlayableItemWidget* radio;
        class PlayableItemWidget* multiRadio;
    } ui;

public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0);

    void update( const lastfm::XmlQuery& user, unsigned int order );
    void setOrder( int order );

    QString name() const;
    QString realname() const;

    bool operator<( const FriendWidget& that ) const;

private:
    void setDetails();

private:
    lastfm::User m_user;
    lastfm::MutableTrack m_recentTrack;
    unsigned int m_order;
};

#endif // FRIENDWIDGET_H
