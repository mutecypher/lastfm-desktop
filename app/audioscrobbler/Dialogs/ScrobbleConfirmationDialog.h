#ifndef SCROBBLECONFIRMATIONDIALOG_H
#define SCROBBLECONFIRMATIONDIALOG_H

#include <lastfm/Track>
#include <QDialog>

#define MIN_WIDTH  500
#define MIN_HEIGHT 500

class ScrobblesModel;

class ScrobbleConfirmationDialog : public QDialog
{
    Q_OBJECT
public:

    ScrobbleConfirmationDialog( const QList<lastfm::Track>& tracks, QWidget* parent = 0 );
    QList<lastfm::Track> tracksToScrobble() const;

private:
    void setupUi();

private slots:
    void toggleSelection();
private:
    struct Ui
    {
        class QDialogButtonBox* buttons;
        class QTableView* scrobblesView;
        class QLabel* infoText;
        class QPushButton* toggleButton;
    }ui;

    ScrobblesModel* m_scrobblesModel;
    bool m_toggled;
};

#endif // SCROBBLECONFIRMATIONDIALOG_H
