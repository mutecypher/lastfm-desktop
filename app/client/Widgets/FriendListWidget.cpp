
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMovie>

#include <lastfm/User.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"
#include "FriendWidget.h"
#include "FriendListWidget.h"
#include "ui_FriendListWidget.h"


class FriendWidgetItem : public QListWidgetItem
{
public:
    FriendWidgetItem( QListWidget* parent );
    bool operator<( const QListWidgetItem& that ) const;
};

FriendWidgetItem::FriendWidgetItem( QListWidget* parent )
    :QListWidgetItem( parent )
{
}

bool
FriendWidgetItem::operator<( const QListWidgetItem& that ) const
{
    return *static_cast<FriendWidget*>(listWidget()->itemWidget( const_cast<FriendWidgetItem*>(this) )) <
            *static_cast<FriendWidget*>(listWidget()->itemWidget( const_cast<QListWidgetItem*>(&that) ));
}

FriendListWidget::FriendListWidget(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::FriendListWidget )
{
    ui->setupUi( this );

    ui->filter->setPlaceholderText( tr( "Search for a friend by username or real name" ) );
    ui->filter->setAttribute( Qt::WA_MacShowFocusRect, false );

    ui->friends->setObjectName( "friends" );
    ui->friends->setAttribute( Qt::WA_MacShowFocusRect, false );
    ui->friends->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    ui->friends->setUniformItemSizes( true );

    connect( ui->filter, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
    connect( aApp, SIGNAL(gotUserInfo(lastfm::User)), SLOT(onGotUserInfo(lastfm::User)) );

    m_movie = new QMovie( ":/loading_meta.gif", "GIF", this );
    ui->spinner->setMovie( m_movie );

    ui->stackedWidget->setCurrentWidget( ui->spinnerPage );
    m_movie->start();

    onSessionChanged( aApp->currentSession() );
}

void
FriendListWidget::onSessionChanged( unicorn::Session* session )
{
    changeUser( session->userInfo().name() );
}


void
FriendListWidget::onGotUserInfo( const lastfm::User& userDetails )
{
    changeUser( userDetails.name() );
}

void
FriendListWidget::changeUser( const QString& newUsername )
{
    if ( !newUsername.isEmpty() && (newUsername != m_currentUsername) )
    {
        ui->friends->clear();

        ui->stackedWidget->setCurrentWidget( ui->spinnerPage );
        m_movie->start();

        m_currentUsername = newUsername;

        connect( User( newUsername ).getFriends( true, 50, 1 ), SIGNAL(finished()), SLOT(onGotFriends()));
    }
}

void
FriendListWidget::onTextChanged( const QString& text )
{
    QString trimmedText = text.trimmed();

    setUpdatesEnabled( false );

    if ( text.isEmpty() )
    {
        // special case an empty string so it's a bit zippier
        for ( int i = 0 ; i < ui->friends->count() ; ++i )
            ui->friends->item( i )->setHidden( false );
    }
    else
    {
        QRegExp re( QString( "^%1" ).arg( trimmedText ), Qt::CaseInsensitive );

        for ( int i = 0 ; i < ui->friends->count() ; ++i )
        {
            FriendWidget* user = static_cast<FriendWidget*>( ui->friends->itemWidget( ui->friends->item( i ) ) );

            ui->friends->item( i )->setHidden( !( user->name().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().split( ' ' ).filter( re ).count() > 0 ) );
        }
    }

    setUpdatesEnabled( true );
}


void FriendListWidget::onCurrentChanged( int index )
{
    if ( index == 3 )
    {
        refresh();
    }
}

void
FriendListWidget::refresh()
{
    connect( User().getFriendsListeningNow( 50, 1 ), SIGNAL(finished()), SLOT(onGotFriendsListeningNow()));
}

void
FriendListWidget::onGotFriends()
{
    // add this set of users to the list
    lastfm::XmlQuery lfm;
    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {

        foreach( const lastfm::XmlQuery& user, lfm["friends"].children( "user" ) )
        {
            FriendWidgetItem* item = new FriendWidgetItem( ui->friends );
            FriendWidget* friendWidget = new FriendWidget( user, this );
            ui->friends->setItemWidget( item, friendWidget );
            item->setSizeHint( friendWidget->sizeHint() );
        }

        int page = lfm["friends"].attribute( "page" ).toInt();
        int perPage = lfm["friends"].attribute( "perPage" ).toInt();
        int totalPages = lfm["friends"].attribute( "totalPages" ).toInt();
        //int total = lfm["friends"].attribute( "total" ).toInt();

        // Check if we need to fetch another page of users
        if ( page != totalPages )
            connect( lastfm::User().getFriends( true, perPage, page + 1 ), SIGNAL(finished()), SLOT(onGotFriends()) );
        else
        {
            // we have fetched all the pages!
            onTextChanged( ui->filter->text() );
            connect( User().getFriendsListeningNow( 50, 1 ), SIGNAL(finished()), SLOT(onGotFriendsListeningNow()));
        }
    }
    else
    {
        // There was an error fetching friends so just show the list

        ui->friends->sortItems( Qt::AscendingOrder );

        if ( ui->friends->count() == 0 )
            ui->stackedWidget->setCurrentWidget( ui->noFriendsPage );
        else
            ui->stackedWidget->setCurrentWidget( ui->friendsPage );

        m_movie->stop();
    }
}


void
FriendListWidget::onGotFriendsListeningNow()
{
    // update the users in the list
    lastfm::XmlQuery lfm;
    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        // reset all the friends to have the same order of max unsigned int
        for ( int i = 0 ; i < ui->friends->count() ; ++i )
            static_cast<FriendWidget*>( ui->friends->itemWidget( ui->friends->item( i ) ) )->setOrder( 0 - 1 );

        QList<XmlQuery> users = lfm["friendslisteningnow"].children( "user" );

        for ( int i = 0 ; i < users.count() ; ++i )
        {
            XmlQuery& user = users[i];

            for ( int j = 0 ; j < ui->friends->count() ; ++j )
            {
                FriendWidget* friendWidget = static_cast<FriendWidget*>( ui->friends->itemWidget( ui->friends->item( j ) ) );

                if ( friendWidget->name().compare( user["name"].text(),Qt::CaseInsensitive ) == 0 )
                    friendWidget->update( user, i );
            }

        }

        int page = lfm["friends"].attribute( "page" ).toInt();
        int perPage = lfm["friends"].attribute( "perPage" ).toInt();
        int totalPages = lfm["friends"].attribute( "totalPages" ).toInt();
        //int total = lfm["friends"].attribute( "total" ).toInt();

        // Check if we need to fetch another page of users
        if ( page != totalPages )
            connect( lastfm::User().getFriends( true, perPage, page + 1 ), SIGNAL(finished()), SLOT(onGotFriends()) );
        else
        {
            // we have fetched all the pages!
            onTextChanged( ui->filter->text() );

            ui->friends->sortItems( Qt::AscendingOrder );

            if ( ui->friends->count() == 0 )
                ui->stackedWidget->setCurrentWidget( ui->noFriendsPage );
            else
                ui->stackedWidget->setCurrentWidget( ui->friendsPage );

            m_movie->stop();
        }
    }
    else
    {
        ui->friends->sortItems( Qt::AscendingOrder );

        if ( ui->friends->count() == 0 )
            ui->stackedWidget->setCurrentWidget( ui->noFriendsPage );
        else
            ui->stackedWidget->setCurrentWidget( ui->friendsPage );

        m_movie->stop();
    }

}
