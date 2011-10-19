#ifndef TOURRADIOPAGE_H
#define TOURRADIOPAGE_H

#include "WizardPage.h"

class TourRadioPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourRadioPage();

private slots:
    void subscribe();

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOURRADIOPAGE_H
