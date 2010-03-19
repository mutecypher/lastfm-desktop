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
#ifndef UNICORN_APPLICATION_H
#define UNICORN_APPLICATION_H

#include "qtsingleapplication/qtsingleapplication.h"

#include "common/HideStupidWarnings.h"
#include "lib/DllExportMacro.h"
#include "UnicornSession.h"
#include <QApplication>
#include "PlayBus.h"
#include <QDebug>

namespace lastfm{ class UserDetails; }
class QNetworkReply;

namespace unicorn
{
    class Bus : public PlayBus
    {
        Q_OBJECT

        public:
            Bus(): PlayBus( "unicorn" )
            {
                connect( this, SIGNAL( message(QByteArray)), SLOT( onMessage(QByteArray)));
                connect( this, SIGNAL( queryRequest( QString, QByteArray )), SLOT( onQuery( QString, QByteArray )));
            };

            bool isSigningIn(){ return sendQuery( "SIGNINGIN" ) == "TRUE"; }
            Session getSession()
            {
                Session s;
                QByteArray ba = sendQuery( "SESSION" ); 
                if( ba.length() > 0 )
                {
                    QDataStream ds( ba );
                    ds >> s;
                }
                return s;
            }

            void changeSession( const Session& s )
            {
                QByteArray ba("");
                QDataStream ds( &ba, QIODevice::WriteOnly | QIODevice::Truncate);
                ds << QByteArray( "SESSIONCHANGED" );
                ds << s;
                sendMessage( ba );
            }
        private slots:

            void onMessage( const QByteArray& message )
            {
                if( message.contains( "SESSIONCHANGED" ))
                {
                    QByteArray sessionData = message.right( message.size() - 18);
                    QDataStream ds( sessionData );
                    Session newSession;
                    ds >> newSession;
                    emit sessionChanged( newSession );
                }
            }

            void onQuery( const QString& uuid, const QByteArray& message )
            {
                if( message == "SIGNINGIN" )
                    emit signingInQuery( uuid );
                else if( message == "SESSION" )
                    emit sessionQuery( uuid );
            }

        signals:
            void signingInQuery( const QString& uuid );
            void sessionQuery( const QString& uuid );
            void sessionChanged( const Session );
    };

    class UNICORN_DLLEXPORT Application : public QtSingleApplication
    {
        Q_OBJECT

        bool m_logoutAtQuit;

    public:
        class StubbornUserException
        {};

        /** will put up the log in dialog if necessary, throwing if the user
          * cancels, ie. they refuse to log in */
        Application( int&, char** ) throw( StubbornUserException );
        ~Application();

        /** Will return the actual stylesheet that is loaded if one is specified
           (on the command-line with -stylesheet or with setStyleSheet(). )
           Note. the QApplication styleSheet property will return the path 
                 to the css file unlike this method. */
        const QString& loadedStyleSheet() const {
            return m_styleSheet;
        }

        Session currentSession() { return m_currentSession; }

        static unicorn::Application* instance(){ return (unicorn::Application*)qApp; }

    public slots:
        bool logout()
        {
            try {
                initiateLogin( true );
            } catch( const StubbornUserException& ) { 
                return false; 
            }
            return true;
        }

        void manageUsers();
        void changeSession( const unicorn::Session& newSession, bool announce = true );

    private:
        void initiateLogin( bool forceLogout = false ) throw( StubbornUserException );
        void translate();
        QString m_styleSheet;
        Session m_currentSession;
        Bus m_bus;
        bool m_signingIn;

    private slots:
        void onUserGotInfo();
        void onSigningInQuery( const QString& );
        void onBusSessionQuery( const QString& );
        void onBusSessionChanged( const Session& );

    signals:
        void gotUserInfo( const lastfm::UserDetails& );
        void sessionChanged( const unicorn::Session& newSession, const unicorn::Session& oldSession );
    };
}

#endif
