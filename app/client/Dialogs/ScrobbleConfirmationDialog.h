/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by MJono Cole and Michael Coffey

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

#ifndef SCROBBLECONFIRMATIONDIALOG_H
#define SCROBBLECONFIRMATIONDIALOG_H

#include <types/Track.h>
#include <QDialog>

#define MIN_WIDTH  500
#define MIN_HEIGHT 500

class ScrobblesModel;

class ScrobbleConfirmationDialog : public QDialog
{
    Q_OBJECT
public:

    ScrobbleConfirmationDialog( const QList<lastfm::Track>& tracks, QWidget* parent = 0 );
    QList<lastfm::Track> tracksToScrobble() const;

private:
    void setupUi();

private slots:
    void toggleSelection();
private:
    struct Ui
    {
        class QDialogButtonBox* buttons;
        class QTableView* scrobblesView;
        class QLabel* infoText;
        class QPushButton* toggleButton;
    }ui;

    ScrobblesModel* m_scrobblesModel;
    bool m_toggled;
};

#endif // SCROBBLECONFIRMATIONDIALOG_H
