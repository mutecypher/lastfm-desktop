#ifndef TOUR_METADATA_PAGE_H
#define TOUR_METADATA_PAGE_H

#include <QWizardPage>
#include <QIcon>

class TourMetadataPage : public QWizardPage {
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    TourMetadataPage( QWidget* );

private:
    void initializePage();
};

#endif // TOUR_METADATA_PAGE_H

