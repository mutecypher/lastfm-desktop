#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>

#include <lastfm/XmlQuery>

#include "lib/unicorn/StylableWidget.h"

class FriendWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class AvatarWidget* avatar;
        class Label* name;
        class Label* details;
        class Label* lastTrack;
        class PlayableItemWidget* radio;
    } ui;

public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0);

    QString name() const;
    QString realname() const;

private slots:
    void onGotInfo();

private:
    const lastfm::XmlQuery m_user;
};

#endif // FRIENDWIDGET_H
