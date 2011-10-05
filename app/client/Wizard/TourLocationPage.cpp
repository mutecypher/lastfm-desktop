#include "TourLocationPage.h"
#include <QTimer>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QBoxLayout>

#include "../Application.h"
#include "../Widgets/PointyArrow.h"

TourLocationPage::TourLocationPage( QWidget* w )
               :QWizardPage( w ),
                m_flash( true )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignCenter );
#ifdef Q_OS_MAC
    ui.image->setObjectName( "imagemac" );
#else
    ui.image->setObjectName( "imagewin" );
#endif
    layout->addWidget( ui.description = new QLabel( tr( "<p>The red arrow on your screen points to the location of the Last.fm Desktop App in your system tray.</p>"
                                                        "<p>Click the icon to quickly access radio play controls, share and tag track, edit your preferences and visit your Last.fm profile.</p>" ), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );

    m_arrow = new PointyArrow;
    m_flashTimer = new QTimer(this);
    m_flashTimer->setInterval( 300 );
    connect( m_flashTimer, SIGNAL(timeout()), SLOT(flashSysTray()));
}


TourLocationPage::~TourLocationPage()
{
    m_flashTimer->stop();
    aApp->tray()->setIcon( m_normalIcon );
    delete m_arrow;
}


void
TourLocationPage::initializePage()
{
#ifdef Q_OS_MAC
    setTitle( tr( "The Last.fm Desktop App in your menu bar" ) );
    ui.image->setPixmap( QPixmap( ":/graphic_location_MAC.png" ) );
#else
    setTitle( tr( "The Last.fm Desktop App in your system tray" ) );
    ui.image->setPixmap( QPixmap( ":/graphic_location_WIN.png" ) );
#endif

    QSystemTrayIcon* tray = aApp->tray();
    m_arrow->pointAt( QPoint( tray->geometry().left() + (tray->geometry().width() / 2.0f ), tray->geometry().top() + (tray->geometry().height() / 2.0f ) ));
    m_flashTimer->start();
    m_normalIcon = tray->icon();
    m_transparentIcon = QPixmap( ":22x22_transparent.png" ).scaled( m_normalIcon.availableSizes().first());
    m_flash = false;

    setButtonText( QWizard::NextButton, tr( "Continue" ) );
    setButtonText( QWizard::BackButton, tr( "<< Back" ) );

    wizard()->setOption( QWizard::HaveCustomButton1, true );
    setButtonText( QWizard::CustomButton1, tr( "Skip Tour >>" ) );
}

void
TourLocationPage::cleanupPage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, false );

    delete m_arrow;
    delete m_flashTimer;
    aApp->tray()->setIcon( m_normalIcon );
}

void
TourLocationPage::flashSysTray()
{
    QSystemTrayIcon* tray = aApp->tray();

    if( m_flash )
        tray->setIcon( m_transparentIcon );
    else
        tray->setIcon( m_normalIcon );

    m_flash = !m_flash;
}
