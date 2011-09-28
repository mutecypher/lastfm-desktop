#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QPointer>


namespace unicorn { class Session; }
namespace lastfm { class XmlQuery; }
namespace lastfm { class UserDetails; }

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

private slots:
    void onSessionChanged( unicorn::Session* session );
    void onGotUserInfo( const lastfm::UserDetails& userDetails );
    void onGotFriends();
    void onTextChanged( const QString& text );

private:
    void changeUser( const QString& newUsername );

private:
    QPointer<QWidget> m_main;

    QString m_currentUsername;
};

#endif // FRIENDLISTWIDGET_H
