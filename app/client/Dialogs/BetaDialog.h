#ifndef BETADIALOG_H
#define BETADIALOG_H

#include <QDialog>

namespace Ui {
class BetaDialog;
}

class BetaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit BetaDialog(QWidget *parent = 0);
    ~BetaDialog();
    
private slots:
    void createIssue();

private:
    Ui::BetaDialog *ui;
};

#endif // BETADIALOG_H
