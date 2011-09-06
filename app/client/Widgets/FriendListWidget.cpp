
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QScrollArea>

#include <lastfm/User.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"
#include "FriendWidget.h"
#include "FriendListWidget.h"

FriendListWidget::FriendListWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( m_main = new QWidget( this ) );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
}

void
FriendListWidget::onSessionChanged( unicorn::Session* session )
{
    layout()->removeWidget( m_main );
    delete m_main;

    connect( session->userInfo().getFriends( true, 50, 1 ), SIGNAL(finished()), SLOT(onGotFriends()));
}


void
FriendListWidget::onTextChanged( const QString& text )
{
    QString trimmedText = text.trimmed();

    setUpdatesEnabled( false );

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.friends->layout());

    if ( text.isEmpty() )
    {
        // special case an empty string so it's a bit zippier
        for ( int i = 1 ; i < layout->count() - 1 ; ++i )
            layout->itemAt( i )->widget()->show();
    }
    else
    {
        QRegExp re( QString( "^%1" ).arg( trimmedText ), Qt::CaseInsensitive );

        // Start from 1 because 0 is the QLineEdit
        // end 1 from the end because the last one is a stretch
        for ( int i = 0 ; i < layout->count() - 1 ; ++i )
        {
            FriendWidget* user = qobject_cast<FriendWidget*>(layout->itemAt( i )->widget());

            layout->itemAt( i )->widget()->setVisible( user->name().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().startsWith( trimmedText, Qt::CaseInsensitive )
                                                       || user->realname().split( ' ' ).filter( re ).count() > 0 );
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

        layout->addWidget( ui.filter = new QLineEdit( this ) );
        ui.filter->setPlaceholderText( tr( "Search for a friend by username or real name" ) );
        ui.filter->setAttribute( Qt::WA_MacShowFocusRect, false );

        layout->addWidget( ui.scrollArea = new QScrollArea( this ) );
        ui.scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui.scrollArea->setWidget( ui.friends = new QWidget( this ) );
        ui.scrollArea->setWidgetResizable( true );

        QVBoxLayout* friendsLayout = new QVBoxLayout( ui.friends );
        friendsLayout->setContentsMargins( 0, 0, 0, 0 );
        friendsLayout->setSpacing( 0 );

        ui.friends->setObjectName( "friends" );

        connect( ui.filter, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
    }

    // add this set of users to the list
    lastfm::XmlQuery lfm;
    lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.friends->layout());

    foreach( const lastfm::XmlQuery& user, lfm["friends"].children( "user" ) )
    {
        bool added = false;

        QString newUser = user["name"].text();

        for ( int i = 0 ; i < layout->count() ; ++i )
        {
            QString listUser = qobject_cast<FriendWidget*>(layout->itemAt( i )->widget())->name();

            if ( newUser.compare( listUser, Qt::CaseInsensitive ) < 0 )
            {
                layout->insertWidget( i, new FriendWidget( user, this ) );
                added = true;
                break;
            }
        }

        if ( !added )
            layout->addWidget( new FriendWidget( user, this ) );

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
        layout->addStretch();
        onTextChanged( ui.filter->text() );
    }

    setUpdatesEnabled( true );
}
