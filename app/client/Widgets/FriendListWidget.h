#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QPointer>


namespace unicorn { class Session; }
namespace lastfm { class XmlQuery; }
namespace lastfm { class User; }

namespace Ui { class FriendListWidget; }

class FriendListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FriendListWidget( QWidget *parent = 0 );

public slots:
    void onCurrentChanged(int);

private slots:
    void onSessionChanged( unicorn::Session* session );
    void onGotUserInfo( const lastfm::User& userDetails );
    void onGotFriends();
    void onGotFriendsListeningNow();
    void onTextChanged( const QString& text );

    void refresh();

private:
    void changeUser( const QString& newUsername );

private:
    QString m_currentUsername;

    Ui::FriendListWidget* ui;
    QPointer<QMovie> m_movie;
};

#endif // FRIENDLISTWIDGET_H
