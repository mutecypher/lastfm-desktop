/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "LoginContinueDialog.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <lastfm/misc.h>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>
#include <QtGui>
#include <QDialogButtonBox>

#ifdef WIN32
#include <windows.h>
#endif


LoginContinueDialog::LoginContinueDialog( QString token, QWidget* parent )
    : QDialog( parent ), m_token( token ), m_subscriber( true )
{
    setWindowModality( Qt::ApplicationModal );

    QVBoxLayout* layout = new QVBoxLayout( this );

    layout->addWidget( ui.title = new QLabel( tr("Are we done?") ) );
    ui.title->setObjectName( "title" );

    layout->addWidget( ui.description = new QLabel( tr("Click OK once you have approved this app.") ) );
    ui.title->setObjectName( "description" );
    ui.title->setWordWrap( true );

    layout->addWidget( ui.buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok ) );

    connect( ui.buttonBox, SIGNAL(accepted()), SLOT(onGetSession()) );
    connect( ui.buttonBox, SIGNAL(rejected()), SLOT(reject()) );
}


void
LoginContinueDialog::cancel()
{
    qDeleteAll( findChildren<QNetworkReply*>() );
}


void
LoginContinueDialog::onGetSession()
{
    connect( unicorn::Session::getSession( m_token ), SIGNAL(finished()), SLOT(onGotSession()) );
}


void
LoginContinueDialog::onGotSession()
{
    try {
        m_session = unicorn::Session( static_cast<QNetworkReply*>(sender()) );
        m_session.setRememberSession( true );
        accept();
    }
    catch (lastfm::ws::ParseError& e)
    {
        qWarning() << e.what();

        switch (e.enumValue())
        {
            case lastfm::ws::AuthenticationFailed:
                // COPYTODO
                QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Login Failed") )
                        .setText( tr("Sorry, we don't recognise that username, or you typed the password wrongly.") )
                        .exec();
                break;
            
            default:
                // COPYTODO
                QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Last.fm Unavailable") )
                        .setText( tr("There was a problem communicating with the Last.fm services. Please try again later.") )
                        .exec();
                break;
            
            case lastfm::ws::TryAgainLater:
            case lastfm::ws::UnknownError:
                switch ((int)static_cast<QNetworkReply*>(sender())->error())
                {
                    case QNetworkReply::ProxyConnectionClosedError:
                    case QNetworkReply::ProxyConnectionRefusedError:
                    case QNetworkReply::ProxyNotFoundError:
                    case QNetworkReply::ProxyTimeoutError:
                    case QNetworkReply::ProxyAuthenticationRequiredError: //TODO we are meant to prompt!
                    case QNetworkReply::UnknownProxyError:
                    case QNetworkReply::UnknownNetworkError:
                        break;
                    default:
                        return;
                }

                // TODO proxy prompting?
                // COPYTODO
                QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Cannot connect to Last.fm") )
                        .setText( tr("Last.fm cannot be reached. Please check your firewall or proxy settings.") )
                        .exec();
                
            #ifdef WIN32
                // show Internet Settings Control Panel
                HMODULE h = LoadLibraryA( "InetCpl.cpl" );
                if (!h) break;
                BOOL (WINAPI *cpl)(HWND) = (BOOL (WINAPI *)(HWND)) GetProcAddress( h, "LaunchConnectionDialog" );
                if (cpl) cpl( winId() );
                FreeLibrary( h );
            #endif
                break;
        }

        reject();
    }
    
#ifdef Q_WS_MAC
    //ui.transient->hide();
#else
    // do last, otherwise it looks weird
    //ui.retranslateUi( this ); //resets Window title
    //ok()->setEnabled( true );
    //ui.spinner->hide();
#endif
}
