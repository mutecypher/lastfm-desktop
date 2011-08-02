#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>

#include <lastfm/XmlQuery>

class FriendWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class HttpImageWidget* avatar;
        class QLabel* name;
        class QLabel* details;
        class QLabel* lastTrack;
        class PlayableItemWidget* radio;
    } ui;

public:
    explicit FriendWidget( const lastfm::XmlQuery& user, QWidget *parent = 0);

    QString name() const;
    QString realname() const;

private slots:
    void onGotInfo();

private:
    const lastfm::XmlQuery m_user;
};

#endif // FRIENDWIDGET_H
