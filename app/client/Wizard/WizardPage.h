#ifndef WIZARDPAGE_H
#define WIZARDPAGE_H

#include <QWidget>

class FirstRunWizard;

class WizardPage : public QWidget
{
    Q_OBJECT
public:
    explicit WizardPage();

    void setWizard( FirstRunWizard* wizard );

    virtual void initializePage() = 0;
    virtual bool validatePage();
    virtual void cleanupPage() = 0;

    void setTitle( const QString& title );

protected:
    FirstRunWizard* wizard() const;

private:
    FirstRunWizard* m_wizard;
};

#endif // WIZARDPAGE_H
