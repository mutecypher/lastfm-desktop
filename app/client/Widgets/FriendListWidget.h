#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QPointer>


namespace unicorn { class Session; }
namespace lastfm { class XmlQuery; }
namespace lastfm { class User; }

class FriendListWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class QLineEdit* filter;
        class QListWidget* friends;
    } ui;

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
    QPointer<QWidget> m_main;

    QString m_currentUsername;
};

#endif // FRIENDLISTWIDGET_H
