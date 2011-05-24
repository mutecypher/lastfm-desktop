/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole, Michael Coffey, and William Viana

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

#include "KeyboardSettingsWidget.h"
#include "../AudioscrobblerSettings.h"
#include "../Application.h"

#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/widgets/UserManagerWidget.h"

#include <lastfm/User>


#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStringListModel>

using lastfm::User;

class ShortCutEdit : public QComboBox {
public:
    ShortCutEdit() {
        setEditable( true );
        QStringList keys;
        keys << QString::fromUtf8("⌃⌘ S")
             << "F1"
             << "F3"
             << "F2"
             << "F4"
             << "F5"
             << "F6"
             << "F7"
             << "F8"
             << "F9";
        QStringListModel* model = new QStringListModel(keys, this);
        setModel(model);
    }

    void setTextValue( QString str ) {
        setCurrentIndex( findText(str));
        lineEdit()->setText( str );
    }

    void keyPressEvent( QKeyEvent* e ) {
        QString text;

        Qt::KeyboardModifiers modifiers;

        //Modifier to symbol
        if( e->modifiers() & Qt::ShiftModifier ) {
            text += QString::fromUtf8( "⇧" );
            modifiers |= Qt::ShiftModifier;
        }
        if( e->modifiers() & Qt::MetaModifier ) {
            text += QString::fromUtf8( "⌃" );
            modifiers |= Qt::MetaModifier;
        }
        if( e->modifiers() & Qt::AltModifier ) {
            text += QString::fromUtf8( "⌥" );
            modifiers |= Qt::AltModifier;
        }
        if( e->modifiers() & Qt::ControlModifier ) {
            text += QString::fromUtf8( "⌘" );
            modifiers |= Qt::ControlModifier;
        }

        //Backspace key to clear shortcut
        if( !e->modifiers() && e->key() == Qt::Key_Backspace) {
            lineEdit()->clear();
            m_modifiers = 0;
            m_key = 0;
            return;
        }
        //Don't allow shortcuts with no modifier unless they're F keys
        if( !e->modifiers() && !( e->key() >= 0x01000030 && e->key() <= 0x01000052 )) return;

        //Don't allow pure modifier shortcuts
        switch(e->key()) {
            case Qt::Key_Control:
            case Qt::Key_Alt:
            case Qt::Key_Meta:
            case Qt::Key_Shift:
                return;
        }

        //Literal Space to Space text
        if( e->key() == Qt::Key_Space)
            text += " Space" ;
        else if( e->key() == Qt::Key_Escape )
            text += QString::fromUtf8(" ⎋");
        else if( e->key() == Qt::Key_Backspace )
            text += QString::fromUtf8(" ⌫");
        else if( e->key() >= 0x01000030 && e->key() <= 0x01000052 )
            text += QString( " F%1" ).arg(((int)e->key() - 0x0100002F) );
        else
            text = text + " " + e->key();

        m_key = e->nativeVirtualKey();
        m_modifiers = e->modifiers();

        setCurrentIndex( findText( text, Qt::MatchFixedString ) );
        lineEdit()->setText( text );
    }

    QString textValue() const{ return lineEdit()->text(); }
    Qt::KeyboardModifiers m_modifiers;
    int m_key;
};

KeyboardSettingsWidget::KeyboardSettingsWidget( QWidget* parent )
    : SettingsWidget( parent )
{
    setupUi();
}

void
KeyboardSettingsWidget::setupUi()
{
    QVBoxLayout* v = new QVBoxLayout();
    QHBoxLayout* h = new QHBoxLayout();

    h->addWidget( new QCheckBox( "Raise Scrobbler"));
    h->addWidget( sce = new ShortCutEdit );

    AudioscrobblerSettings settings;
    sce->setTextValue( settings.raiseShortcutDescription() );
    connect( sce, SIGNAL(editTextChanged(QString)), this, SLOT(onSettingsChanged()));

    v->addLayout(h);

    v->addStretch();
    setLayout( v );
}

void
KeyboardSettingsWidget::saveSettings()
{
    qDebug() << "has unsaved changes?" << hasUnsavedChanges();

    AudioscrobblerSettings settings;
    settings.setRaiseShortcutKey( sce->m_key );
    settings.setRaiseShortcutModifiers( sce->m_modifiers );
    settings.setRaiseShortcutDescription( sce->textValue() );

    aApp->setRaiseHotKey( sce->m_modifiers, sce->m_key );
}
