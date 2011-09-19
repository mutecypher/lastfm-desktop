#ifndef TOUR_LOCATION_PAGE_H
#define TOUR_LOCATION_PAGE_H

#include <QWizardPage>
#include <QIcon>

class TourLocationPage : public QWizardPage {
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    TourLocationPage( QWidget* );
    ~TourLocationPage();

    void initializePage();
    void cleanupPage();

protected slots:
    void flashSysTray();

private:
    class PointyArrow* m_arrow;
    class QTimer* m_flashTimer;
    QIcon m_transparentIcon;
    QIcon m_normalIcon;
    bool m_flash;
};

#endif // TOUR_METADATA_PAGE_H

