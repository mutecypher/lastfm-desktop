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
#include "../widgets/TagBuckets.h"
#include "TagDialog.h"
#include "../widgets/TagListWidget.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/unicorn/widgets/TrackWidget.h"
#include "lib/unicorn/widgets/UnicornTabWidget.h"
#include "lib/unicorn/widgets/ItemSelectorWidget.h"
#include "lib/unicorn/widgets/DataListWidget.h"

#include <lastfm/XmlQuery>
#include <lastfm/User>

#include <QtCore>
#include <QtGui>


TagDialog::TagDialog( const Track& track, QWidget *parent )
        : unicorn::Dialog( parent, Qt::Tool )
{
    m_track = track;
    
    setupUi();
    setWindowTitle( tr("Tag") );
    layout()->setSizeConstraint( QLayout::SetFixedSize );
    setSizeGripEnabled( false );

    connect( User().getTopTags(), SIGNAL(finished()), SLOT(onUserGotTopTags()) );
    connect( m_track.getTopTags(), SIGNAL(finished()), SLOT(onTrackGotTopTags()) );
	
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Tag") );

    connect( ui.buttons, SIGNAL(accepted()), SLOT(onAccepted()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );

    resize( width(), sizeHint().height());
}

void
TagDialog::onUserGotTopTags()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    foreach(const XmlQuery& e, lfm["toptags"].children("tag").mid(0, 5))
    {
        ui.yourTags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }

    ui.yourTagsBox->setEnabled( true );
    if( restoreState( ui.popularTagsBox, "yourTagsToggled", true ).toBool()) {
        ui.yourTagsBox->setChecked( true );
    }

    resize( width(), sizeHint().height());
}

void
TagDialog::onTrackGotTopTags()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    foreach(const XmlQuery& e, lfm["toptags"].children("tag").mid(0, 5))
    {
        ui.popularTags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }
    
    ui.popularTagsBox->setEnabled( true );
    if( restoreState( ui.popularTagsBox, "popularTagsToggled", true ).toBool()) {
        ui.popularTagsBox->setChecked( true );
    }

    resize( width(), sizeHint().height());
}

void
TagDialog::onTagBoxToggled( bool toggled )
{
    QGroupBox* gb = qobject_cast< QGroupBox* >( sender());
    Q_ASSERT( gb );

    if( gb == ui.popularTagsBox )
        saveState( ui.popularTagsBox, "popularTagsToggled", toggled );
    else if( gb == ui.yourTagsBox )
        saveState( ui.yourTagsBox, "yourTagsToggled", toggled );
    
    gb->findChild<QWidget*>()->setVisible( toggled );

    resize( width(), sizeHint().height());
}

void
TagDialog::onAccepted()
{
    // call the ws for tagging
    QNetworkReply* reply;

    if (ui.track->type() == TrackWidget::Track)
        reply = m_track.addTags( ui.tagsWidget->items() );
    else if (ui.track->type() == TrackWidget::Album)
        reply = m_track.album().addTags( ui.tagsWidget->items() );
    else
        reply = m_track.artist().addTags( ui.tagsWidget->items() );

    connect( reply, SIGNAL(finished()), SLOT(onAddTagFinished()));
}

void
TagDialog::onAddTagFinished()
{
    // TODO: parse and see if it was sucessful

    QDialog::accept();
}

void
TagDialog::setupUi()
{   
    QVBoxLayout* v = new QVBoxLayout( this );

    v->addWidget( new QLabel( tr("Add tags") ) );
    v->addWidget( new QLabel( tr("Choose something to tag:") ) );

    v->addWidget( ui.track = new TrackWidget( m_track ) );
    v->addWidget( new QLabel( tr("Tags") ) );

    v->addWidget( ui.tagsWidget = new ItemSelectorWidget( ItemSelectorWidget::Tag, this ) );

    {
        v->addWidget( ui.popularTagsBox = new QGroupBox( tr("Popular tags"), this ) );
        ui.popularTagsBox->setCheckable( true );
        ui.popularTagsBox->setChecked( false );
        ui.popularTagsBox->setEnabled( false );
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->addWidget( ui.popularTags = new DataListWidget );
        ui.popularTags->setHidden( true );
        ui.popularTagsBox->setLayout( layout );
        connect(ui.popularTagsBox, SIGNAL(toggled(bool)), SLOT(onTagBoxToggled(bool)));
    }

    {
        v->addWidget( ui.yourTagsBox = new QGroupBox( tr("Your tags"), this ) );
        ui.yourTagsBox->setCheckable( true );
        ui.yourTagsBox->setChecked( false );
        ui.yourTagsBox->setEnabled( false );
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget( ui.yourTags = new DataListWidget );
        ui.yourTags->setHidden( true );
        layout->setContentsMargins( 0, 0, 0, 0 );
        ui.yourTagsBox->setLayout( layout );
        connect(ui.yourTagsBox, SIGNAL(toggled(bool)), SLOT(onTagBoxToggled(bool)));
    }
    
    v->addStretch();

    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    setTabOrders();
}

void
TagDialog::setTabOrders()
{
    setTabOrder( ui.track, ui.tagsWidget );
    setTabOrder( ui.tagsWidget, ui.buttons->button( QDialogButtonBox::Ok ) );
    setTabOrder( ui.buttons->button( QDialogButtonBox::Ok ), ui.buttons->button( QDialogButtonBox::Cancel ) );
}
