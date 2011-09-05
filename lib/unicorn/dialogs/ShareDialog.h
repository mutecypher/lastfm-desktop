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
#ifndef SHARE_DIALOG_H
#define SHARE_DIALOG_H

#include <lastfm/Track.h>
#include <QDialogButtonBox>
#include "UnicornDialog.h"

#include "lib/DllExportMacro.h"

namespace Ui { class ShareDialog; }

class UNICORN_DLLEXPORT ShareDialog : public unicorn::Dialog
{
    Q_OBJECT
public:

    enum Type
    {
        ShareLastFm,
        ShareTwitter,
        ShareFacebook
    };

    ShareDialog( const Track&, Type type = ShareLastFm, QWidget* parent = 0 );

    Track track() const { return m_track; }

    void setupUi();

private slots:
    void enableDisableOk();
    void accept();
    void onMessageChanged();
    void onShared();
    void updateCharacterLimit();

    void onRadioButtonClicked();

private:
    void setTabOrders();

    QString shareText();
    QUrl shareUrl();
    void setShareText();

private:
    Track m_track;
    Ui::ShareDialog* ui;
    Type m_type;
};

#endif
