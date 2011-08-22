#include <QEvent>
#include <QResizeEvent>
#include <QPainter>

#include "Label.h"

Label::Label( QWidget* parent )
    :QLabel( parent )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setOpenExternalLinks( true );
}


Label::Label( const QString& text, QWidget* parent )
    :QLabel( parent )
{
    setText( text );
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setOpenExternalLinks( true );
}

QString
Label::boldLinkStyle( const QString& text )
{
    return QString( "<html><head><style type=text/css>"
                     "a:link {color:black; font-weight: bold; text-decoration:none;}"
                     "a:hover {color:black; font-weight: bold; text-decoration:none;}"
                     "</style></head><body>%1</body></html>" ).arg( text );
}

void
Label::setText( const QString& text )
{
    m_text = text;

    if ( textFormat() == Qt::RichText )
        QLabel::setText( boldLinkStyle( m_text ) );
    else
        QLabel::setText( ""  );

    update();
}

QString
Label::anchor( const QString& url, const QString& text )
{
    return QString( "<a href=\"%1\">%2</a>" ).arg( url, text );
}

void
Label::paintEvent( QPaintEvent* event )
{
    if ( textFormat() == Qt::RichText )
        QLabel::paintEvent( event );
    else
    {
        QFrame::paintEvent(event);
        QPainter p(this);
        p.drawText( rect(), fontMetrics().elidedText( m_text, Qt::ElideRight, contentsRect().width() ) );
    }
}


