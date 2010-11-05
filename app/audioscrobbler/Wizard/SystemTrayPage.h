#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H

#include <QWizardPage>

class SystemTrayPage : public QWizardPage {
    Q_OBJECT
public:
    SystemTrayPage( QWidget* );
    ~SystemTrayPage();
    virtual void initializePage();
    virtual void cleanupPage();

protected slots:
    void flashSysTray();

protected:
    class PointyArrow* m_arrow;
    class QTimer* m_flashTimer;
};

#endif //SYSTEM_TRAY_H

