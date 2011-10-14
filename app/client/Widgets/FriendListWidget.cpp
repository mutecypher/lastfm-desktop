
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QListWidget>
#include <QListWidgetItem>

#include <lastfm/User.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"
#include "FriendWidget.h"
#include "FriendListWidget.h"



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
    return static_cast<FriendWidget*>(listWidget()->itemWidget( const_cast<FriendWidgetItem*>(this) ))->name().toLower() <
            static_cast<FriendWidget*>(listWidget()->itemWidget( const_cast<QListWidgetItem*>(&that) ))->name().toLower();
}

FriendListWidget::FriendListWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( m_main = new QWidget( this ) );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
    connect( aApp, SIGNAL(gotUserInfo(lastfm::User)), SLOT(onGotUserInfo(lastfm::User)) );

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
        m_currentUsername = newUsername;

        // there's a new username and it's different!
        layout()->removeWidget( m_main );
        delete m_main;

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
        for ( int i = 0 ; i < ui.friends->count() ; ++i )
            ui.friends->item( i )->setHidden( false );
    }
    else
    {
        QRegExp re( QString( "^%1" ).arg( trimmedText ), Qt::CaseInsensitive );

        for ( int i = 0 ; i < ui.friends->count() ; ++i )
        {
            FriendWidget* user = static_cast<FriendWidget*>( ui.friends->itemWidget( ui.friends->item( i ) ) );

            ui.friends->item( i )->setHidden( !( user->name().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().split( ' ' ).filter( re ).count() > 0 ) );
        }
    }

    setUpdatesEnabled( true );
}


void
FriendListWidget::onGotFriends()
{
    setUpdatesEnabled( false );

    // create the layout for all the users if it's not already there
    if ( !m_main )
    {
        layout()->addWidget( m_main = new QWidget( this ) );
        QVBoxLayout* layout = new QVBoxLayout( m_main );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        QWidget* filterBackground = new QWidget( this );
        filterBackground->setObjectName( "filterBackground" );
        layout->addWidget( filterBackground );

        QHBoxLayout* filterLayout = new QHBoxLayout( filterBackground );
        filterLayout->setContentsMargins( 0, 0, 0, 0 );
        filterLayout->setSpacing( 0 );

        filterLayout->addWidget( ui.filter = new QLineEdit( this ) );
        ui.filter->setPlaceholderText( tr( "Search for a friend by username or real name" ) );
        ui.filter->setAttribute( Qt::WA_MacShowFocusRect, false );

        layout->addWidget( ui.friends = new QListWidget( this ) );
        ui.friends->setObjectName( "friends" );
        ui.friends->setAttribute( Qt::WA_MacShowFocusRect, false );
        ui.friends->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
        ui.friends->setUniformItemSizes( true );

        connect( ui.filter, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
    }

    // add this set of users to the list
    lastfm::XmlQuery lfm;
    lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );

    foreach( const lastfm::XmlQuery& user, lfm["friends"].children( "user" ) )
    {
        FriendWidgetItem* item = new FriendWidgetItem( ui.friends );
        FriendWidget* friendWidget = new FriendWidget( user, this );
        ui.friends->setItemWidget( item, friendWidget );
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
        onTextChanged( ui.filter->text() );
    }

    ui.friends->sortItems( Qt::AscendingOrder );

    setUpdatesEnabled( true );
}
