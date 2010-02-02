/*
   Copyright 2005-2009 Last.fm Ltd.

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

#include <QCheckBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "AdvancedOptionsWidget.h"

AdvancedOptionsWidget::AdvancedOptionsWidget(QWidget* parent)
    :QWidget(parent)
{
    QVBoxLayout* optionsLayout = new QVBoxLayout(this);
    {
        QLabel* label = new QLabel(tr("Type of tracks played"));
        label->setObjectName("sliderDescription");
        optionsLayout->addWidget(label, Qt::AlignCenter);       // although it doesn't seem to center :(
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(tr("Obscure")));
        layout->addWidget(m_mainstrSlider = new QSlider(Qt::Horizontal));
        layout->addWidget(new QLabel(tr("Popular")));
        optionsLayout->addLayout(layout);
    }
    {
        QLabel* label = new QLabel(tr("Tracks from the same artist"));
        label->setObjectName("sliderDescription");
        optionsLayout->addWidget(label, Qt::AlignCenter);
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(tr("Less")));
        layout->addWidget(m_repSlider = new QSlider(Qt::Horizontal));
        layout->addWidget(new QLabel(tr("More")));
        optionsLayout->addLayout(layout);
    }

    optionsLayout->addWidget(m_disco = new QCheckBox(tr("Discovery mode")));
    setSupportsDisco( true );

    optionsLayout->addWidget( m_restoreDefaults = new QPushButton( tr("Restore defaults") ) );
    m_restoreDefaults->setObjectName("restoreDefaults");

    connect(m_restoreDefaults, SIGNAL(clicked()), SLOT(onResetDefaultsClicked()));

    setLayout(optionsLayout);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    m_repSlider->setMinimum(0);
    m_repSlider->setMaximum(8);
    m_repSlider->setValue(4);
    m_mainstrSlider->setMinimum(0);
    m_mainstrSlider->setMaximum(8);
    m_mainstrSlider->setValue(4);
}

void
AdvancedOptionsWidget::onResetDefaultsClicked()
{
    setRep( 0.5 );
    setMainstr( 0.5 );
    setDisco( false );
}

void
AdvancedOptionsWidget::setSupportsDisco( bool supportsDisco )
{
    m_disco->setEnabled( supportsDisco );

    if ( supportsDisco )
    {
        m_disco->setToolTip( tr( "This will only play you tracks you've never heard before." ) );
    }
    else
    {
        m_disco->setToolTip( tr( "You've already discovered this music, that's why it's in your library!" ) );
    }
}

void
AdvancedOptionsWidget::setRep(float rep)
{
    m_repSlider->setValue( rep * m_repSlider->maximum() );
}

void
AdvancedOptionsWidget::setMainstr(float mainstr)
{
    m_mainstrSlider->setValue( mainstr * m_mainstrSlider->maximum() );
}

void
AdvancedOptionsWidget::setDisco(bool disco)
{
    m_disco->setChecked( disco );
}

float
AdvancedOptionsWidget::rep() const
{
    return m_repSlider->value() / static_cast<float>(m_repSlider->maximum());
}

float
AdvancedOptionsWidget::mainstr() const
{
    return m_mainstrSlider->value() / static_cast<float>(m_mainstrSlider->maximum());
}

bool
AdvancedOptionsWidget::disco() const
{
    return m_disco->isChecked();
}

