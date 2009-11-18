/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

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
#ifndef LOGIN_PAGE_H_
#define LOGIN_PAGE_H_

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>

#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"

#include <QWizardPage>

class LoginPage : public QWizardPage
{
    Q_OBJECT
public:
    LoginPage( QWidget* parent = 0 )
    :QWizardPage( parent )
    {
        setTitle(tr( "Login" ));
        setSubTitle(tr( "Please enter your last.fm username and password below:" ));
        new QVBoxLayout( this );

        QWidget* login = new QWidget( this );
        QGridLayout* loginLayout = new QGridLayout( login );
        
        ui.username = new QLineEdit( login );
        ui.password = new QLineEdit( login );
        ui.password->setEchoMode( QLineEdit::Password );

        connect( ui.username, SIGNAL(textChanged(QString)), SIGNAL( completeChanged()));
        connect( ui.password, SIGNAL(textChanged(QString)), SIGNAL( completeChanged()));

        loginLayout->addWidget( new QLabel( tr( "Username:" ), login ), 0, 0 );
        loginLayout->addWidget( ui.username, 0, 1 );
        loginLayout->addWidget( new QLabel( tr( "Password:" ), login ), 1, 0 );
        loginLayout->addWidget( ui.password, 1, 1 );

        QLabel* signup = new QLabel( "<a href=\"https://www.last.fm/join\">" + tr( "Sign up for a Last.fm account" ) + "</a>", login);
        signup->setOpenExternalLinks( true );

        loginLayout->addWidget( signup, 2, 0, 1, 2, Qt::AlignTop );

        loginLayout->setRowStretch( 2, 1 );

        layout()->addWidget( login );
    }

    virtual bool isComplete() const
    {
        return !(ui.username->text().isEmpty() || ui.password->text().isEmpty());
    }

private:
    struct {
        QLineEdit* username;
        QLineEdit* password;
    } ui;

};

#endif //LOGIN_PAGE_H_
