
#include <QLabel>
#include <QVBoxLayout>

#include <lastfm/User>
#include <lastfm/XmlQuery>

#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/Widgets/HelpTextLineEdit.h"

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
    setUpdatesEnabled( false );

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_main->layout());

    // Start from 1 because 0 is the HelpTextLineEdit
    // end 1 from the end because the last one is a stretch
    for ( int i = 1 ; i < layout->count() - 1 ; ++i )
    {
        FriendWidget* user = qobject_cast<FriendWidget*>(layout->itemAt( i )->widget());

        layout->itemAt( i )->widget()->setVisible( user->name().contains( text, Qt::CaseInsensitive )
                                                   || user->realname().contains( text, Qt::CaseInsensitive ) );
    }

    setUpdatesEnabled( true );
}


void
FriendListWidget::onGotFriends()
{
    // create the layout for all the users if it's not already there
    if ( !m_main )
    {
        layout()->addWidget( m_main = new QWidget( this ) );
        QVBoxLayout* layout = new QVBoxLayout( m_main );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        HelpTextLineEdit* lineEdit = new HelpTextLineEdit( this );
        layout->addWidget( lineEdit );
        lineEdit->setHelpText( tr( "Search for a friend by username or real name" ) );

        connect( lineEdit, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
    }

    // add this set of users to the list
    lastfm::XmlQuery lfm = qobject_cast<QNetworkReply*>(sender())->readAll();

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_main->layout());

    foreach( const lastfm::XmlQuery& user, lfm["friends"].children( "user" ) )
    {
        bool added = false;

        QString newUser = user["name"].text();

        // Start from 1 because 0 is the HelpTextLineEdit
        // end 1 from the end because the last one is a stretch
        for ( int i = 1 ; i < layout->count() - 1 ; ++i )
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
        layout->addStretch();
}
