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
    explicit TagWidget( const QString& tag, const QString& url, QWidget *parent = 0);

private:
    void paintEvent( QPaintEvent* event );
    QSize sizeHint() const;
    bool event( QEvent* e );

private slots:
    void onClicked();

private:
    QUrl m_url;

    bool m_hovered;

    QPixmap m_left_rest;
    QPixmap m_middle_rest;
    QPixmap m_right_rest;

    QPixmap m_left_hover;
    QPixmap m_middle_hover;
    QPixmap m_right_hover;
};

#endif // TAGWIDGET_H
