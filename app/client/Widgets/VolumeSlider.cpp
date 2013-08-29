#include "VolumeSlider.h"

#include "phonon/AudioOutput"
#include "phonon/VolumeSlider"

#include <QHBoxLayout>
#include <QTimeLine>
#include <QPainter>

VolumeSlider::VolumeSlider( Phonon::AudioOutput* audioOutput, QWidget* parent )
    :QFrame( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.volumeSlider = new Phonon::VolumeSlider( audioOutput, this ), 0, Qt::AlignLeft );

    m_timeLine = new QTimeLine( 500, this );
    m_timeLine->setEasingCurve( QEasingCurve::OutQuint );
    connect( m_timeLine, SIGNAL(frameChanged(int)), SLOT(onFrameChanged(int)));
    connect( m_timeLine, SIGNAL(finished()), SLOT(onFinished()));
}

void
VolumeSlider::paintEvent( QPaintEvent* event )
{
    QFrame::paintEvent( event );

    QPainter p( this );
    p.setPen( Qt::white );
    p.drawLine( rect().topRight() - QPoint( 0, 0 ), rect().bottomRight() - QPoint( 0, 0 ) );
    p.setPen( QColor( 0xcdcdcd ) );
    p.drawLine( rect().topRight() - QPoint( 1, 0 ), rect().bottomRight() - QPoint( 1, 0 ) );
}

void
VolumeSlider::setAudioOutput( Phonon::AudioOutput* audioOutput )
{
    ui.volumeSlider->setAudioOutput( audioOutput );
}

void
VolumeSlider::animateOpen()
{
    m_timeLine->stop();
    m_timeLine->setFrameRange( 2, ui.volumeSlider->size().width() + 10 );
    m_timeLine->start();
    setFixedWidth( 0 );
    show();
}

void
VolumeSlider::animateClose()
{
    m_timeLine->stop();
    m_timeLine->setFrameRange( ui.volumeSlider->size().width() + 10, 2 );
    m_timeLine->start();
}

void
VolumeSlider::onFrameChanged( int frame )
{
    setFixedWidth( frame );
}

void
VolumeSlider::onFinished()
{
    if ( width() == 2 )
        hide();

    parentWidget()->update();
}
