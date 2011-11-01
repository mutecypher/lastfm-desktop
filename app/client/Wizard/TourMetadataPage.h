#ifndef TOUR_METADATA_PAGE_H
#define TOUR_METADATA_PAGE_H

#include "WizardPage.h"
#include <QIcon>

class TourMetadataPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    TourMetadataPage();

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOUR_METADATA_PAGE_H

