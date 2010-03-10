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

#include <QKeyEvent>

#include "SearchBox.h"
#include <QNetworkReply>
#include <QPushButton>
#include <QListView>
#include <QCompleter>
#include <QStringListModel>
#include <lastfm/XmlQuery>
#include <lastfm/Artist>
#include <lastfm/Tag>
#include <lastfm/UserList>

SearchBox::SearchBox(QWidget* parent)
: HelpTextLineEdit( tr(""), parent )
, m_searching(false)
{
    setAttribute( Qt::WA_MacShowFocusRect, false );
    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(m_completer);

    connect(this, SIGNAL(textEdited(QString)), SLOT(onTextEdited(QString)));
}

void
SearchBox::onTextEdited(const QString& text)
{
    if (!m_searching && text.length()) {
        QNetworkReply* reply = startSearch(text);
        if (reply) {
            m_searching = true;
            connect(reply, SIGNAL(finished()), SLOT(onSearchFinished()));
        }
    }
}

void
SearchBox::onSearchFinished()
{
    sender()->deleteLater();
    QString searchTerm;
    try {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        XmlQuery lfm(reply->readAll());
        searchTerm = ((QDomElement)lfm["results"]).attribute("for");
        m_completer->setModel(
            new QStringListModel(
                handleSearchResponse(lfm)));
        m_completer->complete();
    } 
    catch (...) {
    }

    m_searching = false;
    // possibly a search pending:
    if (text() != searchTerm) {
        onTextEdited(text());
    }
}



///////////////////////////////////////////

ArtistSearch::ArtistSearch(QWidget* parent)
: SearchBox(parent)
{
}

QNetworkReply*
ArtistSearch::startSearch(const QString& term)
{
    return Artist(term).search();
}

QStringList
ArtistSearch::handleSearchResponse(XmlQuery& lfm)
{
    QStringList list;
    foreach(XmlQuery i, lfm["results"]["artistmatches"].children("artist")) {
        list << i["name"].text();
    }
    return list;
}

///////////////////////////////////////////

TagSearch::TagSearch(QWidget* parent)
: SearchBox(parent)
{
}

QNetworkReply*
TagSearch::startSearch(const QString& term)
{
    return Tag(term).search();
}

QStringList
TagSearch::handleSearchResponse(XmlQuery& lfm)
{
    QStringList list;
    foreach(XmlQuery i, lfm["results"]["tagmatches"].children("tag")) {
        list << i["name"].text().toLower();
    }
    return list;
}

///////////////////////////////////////////

UserSearch::UserSearch(QWidget* parent)
: SearchBox(parent)
{
    connect(AuthenticatedUser().getFriends(), SIGNAL(finished()), SLOT(onGetFriendsFinished()));
}

int CaseInsensitiveLessThan(const QString& s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

void
UserSearch::onGetFriendsFinished()
{
    lastfm::UserList friendPage = User::list( (QNetworkReply*)sender() );
    m_friends += friendPage;

    if ( friendPage.page == friendPage.totalPages )
    {
        QStringList friends;

        foreach (User u, m_friends)
            friends << u.name();

        qSort(friends.begin(), friends.end(), CaseInsensitiveLessThan);

        m_completer->setCaseSensitivity( Qt::CaseInsensitive );
        m_completer->setModel(new QStringListModel( friends ));
    }
    else
    {
        // get the next page of friends
        connect(AuthenticatedUser().getFriends( friendPage.perPage, friendPage.page + 1 ), SIGNAL(finished()), SLOT(onGetFriendsFinished()));
    }
}

QNetworkReply*
UserSearch::startSearch(const QString& term)
{
    // alas, there is no user.search yet
    Q_UNUSED(term);

    m_completer->complete();
    return 0;
}

QStringList
UserSearch::handleSearchResponse(XmlQuery& lfm)
{
    Q_UNUSED(lfm);
    return QStringList();
}

void
UserSearch::keyPressEvent( QKeyEvent* event )
{
    if ( event->key() == Qt::Key_Backspace )
    {
        if ( text().isEmpty() )
        {
            emit deletePressed();
            return;
        }
    }
    else if ( event->text() == "," )
    {
        emit commaPressed();
        return;
    }

    QLineEdit::keyPressEvent( event );
}
