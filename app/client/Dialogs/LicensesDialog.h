#ifndef LICENCEDIALOG_H
#define LICENCEDIALOG_H

#include <QDialog>

namespace Ui {
class LicensesDialog;
}

class LicensesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LicensesDialog(QWidget *parent = 0);
    ~LicensesDialog();
    
private:
    Ui::LicensesDialog *ui;
};

#endif // LICENCEDIALOG_H
