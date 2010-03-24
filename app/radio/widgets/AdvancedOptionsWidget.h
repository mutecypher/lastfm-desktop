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

#ifndef ADVANCED_OPTIONS_WIDGET_H
#define ADVANCED_OPTIONS_WIDGET_H

#include <QWidget>

class QSlider;
class QCheckBox;
class QPushButton;

class AdvancedOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    AdvancedOptionsWidget(QWidget* parent = 0);

    void setRep(float rep);
    void setMainstr(float mainstr);
    void setDisco(bool disco);

    float rep() const;
    float mainstr() const;
    bool disco() const;

public slots:
    void setSupportsDisco( bool supportsDisco );
    void onResetDefaultsClicked();

private:
    QWidget* m_sliders;
    QSlider* m_repSlider;
    QSlider* m_mainstrSlider;
    QCheckBox* m_disco;
    QPushButton* m_restoreDefaults;
};

#endif // ADVANCED_OPTIONS_WIDGET_H
