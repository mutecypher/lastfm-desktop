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
#include "LoginPage.h"
#include "../Application.h"

#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDesktopServices>

void
findAndHideWizardRuler( QObject* object )
{
    qDebug() << object->metaObject()->className();

    QLayout* l = qobject_cast<QLayout*>(object);

    if ( l )
    {
        for ( int i = 0 ; i < l->count() ; ++i )
        {
            QWidget* widget = l->itemAt( i )->widget();

            if ( widget )
            {
                if ( QString( "QWizardRuler" ).compare( widget->metaObject()->className() ) == 0 )
                    widget->hide();

                findAndHideWizardRuler( widget );
            }


        }
    }

    if ( object->isWidgetType() )
    {
        QLayout* layout = qobject_cast<QWidget*>( object )->layout();

        if ( layout )
        {
            for ( int i = 0 ; i < layout->count() ; ++i )
            {
                QWidget* widget = layout->itemAt( i )->widget();

                if ( widget )
                {
                    if ( QString( "QWizardRuler" ).compare( widget->metaObject()->className() ) == 0 )
                        widget->hide();

                    findAndHideWizardRuler( widget );
                }
            }
        }
    }

    foreach ( QObject* child, object->children() )
    {
        qDebug() << child->metaObject()->className();

        if ( QString( child->metaObject()->className() ) == QString( "QWizardRuler" ) )
            qobject_cast<QWidget*>(child)->hide();

        findAndHideWizardRuler( child );
    }
}

LoginPage::LoginPage( QWidget* parent )
          :QWizardPage( parent )
          , m_loginProcess( 0 )
          , m_isComplete( false )
{
    setTitle( tr( "Hello! Let's get started by connecting your Last.fm account" ) );

    QHBoxLayout* layout = new QHBoxLayout( this );
    
    layout->addWidget( ui.image = new QLabel( this ), 0 );
    ui.image->setObjectName( "image" );

    layout->addWidget( ui.description = new QLabel( tr( "<p>Already a Last.fm user? You can connect the Last.fm Desktop App to your profile and keep a record of the music you listen to.</p>"
                                                        "<p>If you don't have an account you can sign up now for free.</p>" ) ), 0, Qt::AlignTop );
    
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
LoginPage::initializePage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, true );
    connect( wizard()->button( QWizard::CustomButton1 ), SIGNAL( clicked()), SLOT( onSignUpClicked()));
    setButtonText( QWizard::NextButton, tr( "Connect Your Account" ) );
    setButtonText( QWizard::CustomButton1, tr( "Sign up" ));

    QTimer::singleShot( 200, this, SLOT(yah()));
}

void
LoginPage::yah()
{
    findAndHideWizardRuler( wizard() );
}

void 
LoginPage::cleanupPage()
{
    disconnect( wizard()->button( QWizard::CustomButton1 ), SIGNAL( clicked()), this, 0 );
}

bool 
LoginPage::validatePage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, false );
    return true;
}

bool
LoginPage::isComplete() const
{
    return true;
}

void 
LoginPage::onSignUpClicked()
{
    QDesktopServices::openUrl( QUrl( "http://www.last.fm/join" ));
}
