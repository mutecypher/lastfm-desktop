#ifndef WELCOME_PAGE_H
#define WELCOME_PAGE_H

#include "lib/unicorn/UnicornApplication.h"

#include "lib/unicorn/Updater/PluginList.h"
#include "lib/unicorn/UnicornSession.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>
#include <QWizardPage>

class WelcomePage : public QWizardPage
{
    Q_OBJECT
public:
    WelcomePage( QWidget* parent = 0 );

    virtual void initializePage();
    virtual void cleanupPage();

private:
    struct Ui
    {
        QLabel* welcomeLabel;
    }ui;
};

#endif // WELCOME_PAGE_H
