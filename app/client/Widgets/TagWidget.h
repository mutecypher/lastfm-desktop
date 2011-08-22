#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>

class TagWidget : public QPushButton
{
    Q_OBJECT
public:
    Q_PROPERTY( QPixmap left READ left WRITE setLeft );
    Q_PROPERTY( QPixmap middle READ middle WRITE setMiddle );
    Q_PROPERTY( QPixmap right READ right WRITE setRight );

    explicit TagWidget( const QString& tag, const QString& url, QWidget *parent = 0);

    const QPixmap* left() const;
    void setLeft( const QPixmap& left );

    const QPixmap* middle() const;
    void setMiddle( const QPixmap& middle );

    const QPixmap* right() const;
    void setRight( const QPixmap& right );

private:
    void paintEvent( QPaintEvent* event );
    QSize sizeHint() const;

private slots:
    void onClicked();

private:
    QUrl m_url;

    QPixmap m_left;
    QPixmap m_middle;
    QPixmap m_right;
};

#endif // TAGWIDGET_H
