#include "SimilarArtistWidget.h"

SimilarArtistWidget::SimilarArtistWidget(QWidget *parent) :
    HttpImageWidget(parent)
{
}

void
SimilarArtistWidget::setArtist( const QString& artist )
{
    m_artist = artist;
}

void
SimilarArtistWidget::paintEvent( QPaintEvent* event )
{
    HttpImageWidget::paintEvent( event );

    QPainter p;
    p.begin( this );

    QTextOption to;

    to.setAlignment( Qt::AlignBottom | Qt::AlignLeft );

    QFontMetrics fm( font() );

    QRect rect = contentsRect().adjusted( -3, 0, 13, 22 );

    p.drawText( rect, fm.elidedText( m_artist, Qt::ElideRight, rect.width() ), to );

    p.end();
}
