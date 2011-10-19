#ifndef TOURSCROBBLESPAGE_H
#define TOURSCROBBLESPAGE_H

#include "WizardPage.h"

class TourScrobblesPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourScrobblesPage();

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOURSCROBBLESPAGE_H
