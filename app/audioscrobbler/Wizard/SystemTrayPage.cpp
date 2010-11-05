#include "SystemTrayPage.h"
#include <QTimer>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QBoxLayout>

#include "../Application.h"
#include "../Widgets/PointyArrow.h"

SystemTrayPage::SystemTrayPage( QWidget* w )
               :QWizardPage( w )
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
    aApp->tray()->show();
    delete m_arrow;
}


void 
SystemTrayPage::initializePage()
{
    QSystemTrayIcon* tray = aApp->tray();
    m_arrow->pointAt( QPoint( tray->geometry().left() + (tray->geometry().width() / 2.0f ), 0 ));
    m_flashTimer->start();
}

void
SystemTrayPage::cleanupPage()
{
    m_arrow->hide();
    m_flashTimer->stop();
    aApp->tray()->show();
}

void
SystemTrayPage::flashSysTray()
{
    QSystemTrayIcon* tray = aApp->tray();

    if( tray->isVisible() ) {
        tray->hide();
    } else {
        tray->show();
    }
}
