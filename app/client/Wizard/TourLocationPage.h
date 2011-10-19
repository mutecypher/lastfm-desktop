#ifndef TOUR_LOCATION_PAGE_H
#define TOUR_LOCATION_PAGE_H

#include "WizardPage.h"
#include <QIcon>
#include <QPointer>

class PointyArrow;

class TourLocationPage : public WizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    TourLocationPage();
    ~TourLocationPage();

    void initializePage();
    void cleanupPage();

protected slots:
    void flashSysTray();

private:
    QPointer<PointyArrow> m_arrow;
    QPointer<QTimer> m_flashTimer;
    QIcon m_transparentIcon;
    QIcon m_normalIcon;
    bool m_flash;
};

#endif // TOUR_METADATA_PAGE_H

