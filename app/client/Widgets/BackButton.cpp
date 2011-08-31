
#include <QPainter>
#include <QFont>

#include "BackButton.h"

BackButton::BackButton(QWidget *parent) :
    QPushButton(parent)
{
}

void
BackButton::setDescription( const QString& description )
{
    m_description = description;
}

void
BackButton::paintEvent( QPaintEvent* e )
{
    QPushButton::paintEvent( e );

    QPainter p( this );

    QFont font = p.font();
    font.setWeight( QFont::Bold );
    font.setPixelSize( 14 );
    p.setFont( font );

    QTextOption to;
    to.setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
    p.drawText( rect().adjusted( 50, 16, 0, 0 ), m_description, to );
}

