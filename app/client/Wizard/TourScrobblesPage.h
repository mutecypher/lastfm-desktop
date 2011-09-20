#ifndef TOURSCROBBLESPAGE_H
#define TOURSCROBBLESPAGE_H

#include <QWizardPage>

class TourScrobblesPage : public QWizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourScrobblesPage( QWidget* parent = 0 );

private:
    void initializePage();
    void cleanupPage();
};

#endif // TOURSCROBBLESPAGE_H
