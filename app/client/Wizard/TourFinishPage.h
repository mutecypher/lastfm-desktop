#ifndef TOURFINISHPAGE_H
#define TOURFINISHPAGE_H

#include "WizardPage.h"

class TourFinishPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourFinishPage();

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOURFINISHPAGE_H
