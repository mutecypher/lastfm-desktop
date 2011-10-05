#ifndef TOURRADIOPAGE_H
#define TOURRADIOPAGE_H

#include <QWizardPage>

class TourRadioPage : public QWizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourRadioPage( QWidget* parent = 0 );

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOURRADIOPAGE_H
