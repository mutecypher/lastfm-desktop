#ifndef TOURFINISHPAGE_H
#define TOURFINISHPAGE_H

#include <QWizardPage>

class TourFinishPage : public QWizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit TourFinishPage(QWidget *parent = 0);

private:
    void initializePage();

};

#endif // TOURFINISHPAGE_H
