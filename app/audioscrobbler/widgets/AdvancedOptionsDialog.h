/*
   Copyright 2005-2010 Last.fm Ltd.

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

#ifndef ADVANCED_OPTIONS_DIALOG_H
#define ADVANCED_OPTIONS_DIALOG_H

#include <QDialog>

class AdvancedOptionsWidget;

class AdvancedOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    AdvancedOptionsDialog(QWidget* parent = 0);

    AdvancedOptionsWidget& widget() const { return *m_widget; }

private:
    AdvancedOptionsWidget* m_widget;
};

#endif // ADVANCED_OPTIONS_DIALOG_H
