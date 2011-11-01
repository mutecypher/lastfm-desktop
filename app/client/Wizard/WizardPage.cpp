

#include "FirstRunWizard.h"

#include "WizardPage.h"

WizardPage::WizardPage()
{
}

bool
WizardPage::validatePage()
{
    return true;
}


void
WizardPage::setTitle( const QString &title )
{
    wizard()->setTitle( title );
}

void
WizardPage::setWizard( FirstRunWizard* wizard )
{
    m_wizard = wizard;
}

FirstRunWizard*
WizardPage::wizard() const
{
    return m_wizard;
}
