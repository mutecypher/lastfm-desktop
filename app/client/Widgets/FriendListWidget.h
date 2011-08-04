#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QPointer>

namespace unicorn { class Session; };
namespace lastfm { class XmlQuery; };

class FriendListWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class QLineEdit* filter;
    } ui;

public:
    explicit FriendListWidget(QWidget *parent = 0);

signals:

private slots:
    void onSessionChanged( unicorn::Session* session );
    void onGotFriends();
    void onTextChanged( const QString& text );

private:
    QPointer<QWidget> m_main;
};

#endif // FRIENDLISTWIDGET_H
