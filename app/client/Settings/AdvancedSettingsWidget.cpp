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

#include "ui_AdvancedSettingsWidget.h"
#include "AdvancedSettingsWidget.h"
#include "../Widgets/ShortcutEdit.h"
#include "../AudioscrobblerSettings.h"
#include "../Application.h"

#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/widgets/UserManagerWidget.h"

#include <lastfm/User.h>


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

AdvancedSettingsWidget::AdvancedSettingsWidget( QWidget* parent )
    : SettingsWidget( parent ),
      ui( new Ui::AdvancedSettingsWidget )
{
    ui->setupUi( this );

    AudioscrobblerSettings settings;
    ui->sce->setTextValue( settings.raiseShortcutDescription() );
    ui->sce->setModifiers( settings.raiseShortcutModifiers() );
    ui->sce->setKey( settings.raiseShortcutKey() );

    connect( ui->sce, SIGNAL(editTextChanged(QString)), this, SLOT(onSettingsChanged()));
}

void
AdvancedSettingsWidget::saveSettings()
{
    qDebug() << "has unsaved changes?" << hasUnsavedChanges();

    AudioscrobblerSettings settings;
    settings.setRaiseShortcutKey( ui->sce->key() );
    settings.setRaiseShortcutModifiers( ui->sce->modifiers() );
    settings.setRaiseShortcutDescription( ui->sce->textValue() );

    aApp->setRaiseHotKey( ui->sce->modifiers(), ui->sce->key() );
}
