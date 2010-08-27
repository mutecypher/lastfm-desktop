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
#include <QMainWindow>

namespace lastfm{
    class UserDetails;
    class InternetConnectionMonitor;
}

class LoginContinueDialog;

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

            bool isWizardRunning(){ return sendQuery( "WIZARDRUNNING" ) == "TRUE"; }

            QDataStream getSessionData()
            {
                QByteArray ba = sendQuery( "SESSION" ); 
                if( ba.length() > 0 )
                {
                    QDataStream ds( ba );
                    return ds;
                }
                return QDataStream();
            }

            void announceSessionChange( Session* s )
            {
                qDebug() << "Session change, let's spread the message through the bus!";
                QByteArray ba; 
                QDataStream ds( &ba, QIODevice::WriteOnly | QIODevice::Truncate );

                ds << QString( "SESSIONCHANGED" );
                ds << ( *s );
                                
                sendMessage( ba );
            }
        private slots:

            void onMessage( const QByteArray& message )
            {
                qDebug() << "Message received";
                qDebug() << "Message: " << message;
                QDataStream ds( message );
                QString stringMessage;

                ds >> stringMessage;
        
                if( stringMessage == "SESSIONCHANGED" )
                {
                    qDebug() << "and it's a session change alert";
                    Session* newSession = new Session();
                    ds >> ( *newSession );
                    emit sessionChanged( newSession );
                }
                else if( message.startsWith( "LOVED=" ))
                {
                    QByteArray sessionData = message.right( message.size() - 6);
                    emit lovedStateChanged( sessionData == "true" );
                }
            }

            void onQuery( const QString& uuid, const QByteArray& message )
            {
                if( message == "WIZARDRUNNING" )
                    emit wizardRunningQuery( uuid );
                else if( message == "SESSION" )
                    emit sessionQuery( uuid );
            }

        signals:
            void wizardRunningQuery( const QString& uuid );
            void sessionQuery( const QString& uuid );
            void sessionChanged( Session* );
            void rosterUpdated();
            void lovedStateChanged(bool loved);
    };

    /**
     * Unicorn base Application.
     *
     * Child classes should make sure to call the protected function initiateLogin
     * on their constructor otherwise the app would probably crash, as there will be
     * no valid user session.
     */
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

        virtual void init();

        /** Will return the actual stylesheet that is loaded if one is specified
           (on the command-line with -stylesheet or with setStyleSheet(). )
           Note. the QApplication styleSheet property will return the path 
                 to the css file unlike this method. */
        const QString& loadedStyleSheet() const {
            return m_styleSheet;
        }

        Session* currentSession() { return m_currentSession; }

        static unicorn::Application* instance(){ return (unicorn::Application*)qApp; }
        void installHotKey( Qt::KeyboardModifiers, quint32, QObject* receiver, const char* slot );

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
        void changeSession( unicorn::Session* newSession, bool announce = true );
        void sendBusLovedStateChanged(bool loved);
        void refreshStyleSheet();
        void restart();

    private:
        void translate();
        void setupHotKeys();
        void onHotKeyEvent(quint32 id);
        void loadStyleSheet( QFile& );
        QMainWindow* findMainWindow();

        QString m_styleSheet;
        Session* m_currentSession;
        bool m_wizardRunning;
        QMap< quint32, QPair<QObject*, const char*> > m_hotKeyMap;
        QString m_cssDir;
        QString m_cssFileName;

#ifdef __APPLE__
        static short appleEventHandler( const AppleEvent*, AppleEvent*, long );
        static OSStatus hotkeyEventHandler( EventHandlerCallRef, EventRef, void* );
#endif
#ifdef WIN32
        static bool winEventFilter ( void* );
#endif
    protected:
        /**
         * Reimplement this function if you want to control the initial login process.
         */
        virtual void initiateLogin( bool forceLogout = false ) throw( StubbornUserException );

        void setWizardRunning( bool running );

        Bus m_bus;
        lastfm::InternetConnectionMonitor* m_icm;
	

    private slots:
        void onUserGotInfo();
        void onWizardRunningQuery( const QString& );
        void onBusSessionQuery( const QString& );
        void onBusSessionChanged( Session* );

    signals:
        void gotUserInfo( const lastfm::UserDetails& );
        void sessionChanged( unicorn::Session* newSession, unicorn::Session* oldSession );
        void rosterUpdated();
        void busLovedStateChanged(bool loved);
        void internetConnectionUp();
        void internetConnectionDown();
    };
}

#endif
