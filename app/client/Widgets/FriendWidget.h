#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>

#include <lastfm/XmlQuery.h>
#include <lastfm/User.h>

#include "lib/unicorn/StylableWidget.h"

class FriendWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class AvatarWidget* avatar;
        class QLabel* details;
        class PlayableItemWidget* radio;
        class PlayableItemWidget* multiRadio;
    } ui;

public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0);

    QString name() const;
    QString realname() const;

private slots:
    void onGotInfo();

private:
    void setDetails();

private:
    const lastfm::XmlQuery m_user;
    lastfm::UserDetails m_userDetails;
};

#endif // FRIENDWIDGET_H
