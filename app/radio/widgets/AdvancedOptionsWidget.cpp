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
    QGridLayout* layout = new QGridLayout(this);
    {
        QLabel* label = new QLabel(tr("Type of tracks played"));
        label->setObjectName("sliderDescription");
        layout->addWidget(label, 0, 0, 1, 3, Qt::AlignCenter);
        layout->addWidget(new QLabel(tr("Obscure")), 1, 0, 1, 1, Qt::AlignRight);
        layout->addWidget(m_mainstrSlider = new QSlider(Qt::Horizontal), 1, 1, 1, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel(tr("Popular")), 1, 2, 1, 1, Qt::AlignLeft);
    }
    {
        QLabel* label = new QLabel(tr("Tracks from the same artist"));
        label->setObjectName("sliderDescription");
        layout->addWidget(label, 2, 0, 1, 3, Qt::AlignCenter);
        layout->addWidget(new QLabel(tr("Less")), 3, 0, 1, 1, Qt::AlignRight);
        layout->addWidget(m_repSlider = new QSlider(Qt::Horizontal), 3, 1, 1, 1, Qt::AlignCenter);
        layout->addWidget(new QLabel(tr("More")), 3, 2, 1, 1, Qt::AlignLeft);
    }

    layout->addWidget(m_disco = new QCheckBox(tr("Discovery mode")), 4, 0, 1, 3, Qt::AlignCenter);
    setSupportsDisco( true );

    layout->addWidget( m_restoreDefaults = new QPushButton( tr("Restore defaults") ), 5, 0, 1, 3, Qt::AlignCenter );
    m_restoreDefaults->setObjectName("restoreDefaults");

    layout->setRowStretch ( 0, 0 );
    layout->setRowStretch ( 1, 1 );
    layout->setRowStretch ( 2, 0 );

    connect(m_restoreDefaults, SIGNAL(clicked()), SLOT(onResetDefaultsClicked()));

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

