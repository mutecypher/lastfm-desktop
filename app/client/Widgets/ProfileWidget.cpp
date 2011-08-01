
#include "../Application.h"

#include "ProfileWidget.h"

ProfileWidget::ProfileWidget(QWidget *parent)
    :QWidget(parent)
{
    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
}

void
ProfileWidget::onSessionChanged( unicorn::Session* session )
{

}
