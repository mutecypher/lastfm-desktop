#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>
#include <QPointer>

#include <lastfm/XmlQuery.h>
#include <lastfm/User.h>
#include <lastfm/Track.h>

#include "lib/unicorn/StylableWidget.h"

namespace Ui { class FriendWidget; }
namespace unicorn { class Label; }
using unicorn::Label;

class FriendWidget : public StylableWidget
{
    Q_OBJECT
public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0 );

    void update( const lastfm::XmlQuery& user, unsigned int order );
    void setOrder( int order );

    QString name() const;
    QString realname() const;

    void setListeningNow( bool listeningNow );

    bool operator<( const FriendWidget& that ) const;

signals:
    void sizeChanged( const QSize& size );

private:
    void setDetails();

    void resizeEvent(QResizeEvent *);

private:
    Ui::FriendWidget* ui;

    lastfm::User m_user;
    lastfm::MutableTrack m_recentTrack;
    unsigned int m_order;
    bool m_listeningNow;

    QPointer<QMovie> m_movie;
};

#endif // FRIENDWIDGET_H
