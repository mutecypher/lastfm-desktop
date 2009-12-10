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

    setLayout(optionsLayout);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    m_repSlider->setMinimum(0);
    m_repSlider->setMaximum(8);
    m_repSlider->setValue(4);
    m_mainstrSlider->setMinimum(0);
    m_mainstrSlider->setMaximum(8);
    m_mainstrSlider->setValue(4);
}

QString
AdvancedOptionsWidget::rqlOptions() const
{
    QString rqlOptions;

    float r = m_repSlider->value() / (float) m_repSlider->maximum();
    if (r != 0.5) {
        rqlOptions += QString(" opt:rep|%1").arg(r);
    }
    float m = m_mainstrSlider->value() / (float) m_mainstrSlider->maximum();
    if (m != 0.5) {
        rqlOptions += QString(" opt:mainstr|%1").arg(m);
    }
    if (m_disco->isChecked()) {
        rqlOptions += QString(" opt:discovery|true");
    }

    return rqlOptions;
}

