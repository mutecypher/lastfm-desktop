#include "SystemTrayPage.h"
#include <QTimer>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QBoxLayout>

#include "../Application.h"
#include "../Widgets/PointyArrow.h"

SystemTrayPage::SystemTrayPage( QWidget* w )
               :QWizardPage( w ),
                flash( true )
{
    m_arrow = new PointyArrow;
    m_flashTimer = new QTimer(this);
    m_flashTimer->setInterval( 300 );
    connect( m_flashTimer, SIGNAL( timeout()), SLOT( flashSysTray()));

    new QVBoxLayout( this );
    
    QLabel* screenshot;
    layout()->addWidget( screenshot = new QLabel( this ));
    screenshot->setPixmap( QPixmap( ":/systray_screenshot.png" ));
}


SystemTrayPage::~SystemTrayPage()
{
    m_flashTimer->stop();
    aApp->tray()->setIcon( m_normalIcon );
    delete m_arrow;
}


void 
SystemTrayPage::initializePage()
{
    QSystemTrayIcon* tray = aApp->tray();
    m_arrow->pointAt( QPoint( tray->geometry().left() + (tray->geometry().width() / 2.0f ), tray->geometry().top() + (tray->geometry().height() / 2.0f ) ));
    m_flashTimer->start();
	m_normalIcon = tray->icon();
    m_transparentIcon = QPixmap( ":22x22_transparent.png" ).scaled( m_normalIcon.availableSizes().first());
	flash = false;
}

void
SystemTrayPage::cleanupPage()
{
    m_arrow->hide();
    m_flashTimer->stop();
    aApp->tray()->setIcon( m_normalIcon );
}

void
SystemTrayPage::flashSysTray()
{
    QSystemTrayIcon* tray = aApp->tray();

    if( flash ){
        tray->setIcon( m_transparentIcon );
    } else {
        tray->setIcon( m_normalIcon );
    }
	flash = !flash;
}
