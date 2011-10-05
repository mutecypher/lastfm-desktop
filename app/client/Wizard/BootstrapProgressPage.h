#ifndef BOOTSTRAPPROGRESSPAGE_H
#define BOOTSTRAPPROGRESSPAGE_H

#include <QWizardPage>

class BootstrapProgressPage : public QWizardPage
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
    } ui;

public:
    explicit BootstrapProgressPage( QWidget* parent = 0 );

private:
    void initializePage();
};

#endif // BOOTSTRAPPROGRESSPAGE_H
