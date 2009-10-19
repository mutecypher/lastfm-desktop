#ifndef ANIMATED_PUSHBUTTON_H
#define ANIMATED_PUSHBUTTON_H

#include <QPushButton>
#include <QMovie>
#include <QDebug>
#include <iostream>
class AnimatedPushButton : public QPushButton
{
public:
    AnimatedPushButton( QMovie* movie, const QString& text, QWidget* parent = 0 )
    :QPushButton( text, parent ),
     m_movie( movie )
    {
        m_movie->start();
        setIcon(QIcon( m_movie->currentPixmap()));
        m_movie->stop();
    }

private:
    QMovie* m_movie;
};

#endif //ANIMATED_PUSHBUTTON_H
