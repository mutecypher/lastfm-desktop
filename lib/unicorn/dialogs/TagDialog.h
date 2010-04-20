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
#ifndef TAG_DIALOG_H
#define TAG_DIALOG_H

#include <lastfm/Track>
#include <QModelIndex>
#include "UnicornDialog.h"

#include "lib/DllExportMacro.h"

namespace unicorn
{
    class TabWidget;
}


class UNICORN_DLLEXPORT TagDialog : public unicorn::Dialog
{
    Q_OBJECT

public:
    TagDialog( const Track&, QWidget* parent );

	Track track() const { return m_track; }

private:
    void setupUi();

private slots:
    void onUserGotTopTags();
    void onTrackGotTopTags();
    void onTagBoxToggled(bool);

private:
    struct Ui
    {
        class TrackWidget* track;
        class ItemSelectorWidget* tagsWidget;
        class QGroupBox* popularTagsBox;
        class DataListWidget* popularTags;
        class QGroupBox* yourTagsBox;
        class DataListWidget* yourTags;
        class QDialogButtonBox* buttons;
        
        void setupUi( QWidget* parent );
    } ui;
    
    Track m_track;
    QStringList m_originalTags;
    QStringList m_publicTags;
    QStringList m_userTags;
};

#endif
