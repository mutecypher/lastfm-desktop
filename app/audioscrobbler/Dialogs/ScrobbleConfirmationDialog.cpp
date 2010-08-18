#include "ScrobbleConfirmationDialog.h"

#include "../ScrobblesModel.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

ScrobbleConfirmationDialog::ScrobbleConfirmationDialog( const QList<Track>& tracks, QWidget* parent )
    : QDialog( parent )
{
    m_toggled = true;
    m_scrobblesModel = new ScrobblesModel( tracks, this );

    setupUi();
}

void
ScrobbleConfirmationDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );

    ui.infoText = new QLabel( tr( "These are the tracks you are about to scrobble." ), this );
    ui.scrobblesView = new QTableView( this );
    ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    ui.toggleButton = new QPushButton( tr( "Toggle selection" ), this );

    //allow sorting
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel( this );
    proxyModel->setSourceModel( m_scrobblesModel );

    ui.scrobblesView->setModel( proxyModel );

    ui.scrobblesView->resizeColumnsToContents();
    ui.scrobblesView->resizeRowsToContents();
    ui.scrobblesView->setShowGrid( false );
    ui.scrobblesView->verticalHeader()->hide();
    ui.scrobblesView->horizontalHeader()->setResizeMode( QHeaderView::Stretch );

    ui.scrobblesView->setSelectionMode( QAbstractItemView::NoSelection );

    ui.scrobblesView->hideColumn( ScrobblesModel::Loved );
    ui.scrobblesView->hideColumn( ScrobblesModel::Album );

    ui.scrobblesView->setSortingEnabled( true );

    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    buttonsLayout->addWidget( ui.toggleButton );
    buttonsLayout->addStretch( 1 );
    buttonsLayout->addWidget( ui.buttons );

    mainLayout->addWidget( ui.infoText );
    mainLayout->addWidget( ui.scrobblesView );
    mainLayout->addLayout( buttonsLayout );

    connect( ui.buttons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( ui.buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( ui.toggleButton, SIGNAL( clicked() ), this, SLOT( toggleSelection() ) );

    setMinimumSize( MIN_WIDTH, MIN_HEIGHT );
}

QList<Track>
ScrobbleConfirmationDialog::tracksToScrobble() const
{
    return m_scrobblesModel->tracksToScrobble();
}

void
ScrobbleConfirmationDialog::toggleSelection()
{
    m_toggled = !m_toggled;
    for( int i = 0; i < m_scrobblesModel->rowCount(); i++ )
    {
        QModelIndex idx = m_scrobblesModel->index( i, ScrobblesModel::Artist, QModelIndex() );
        m_scrobblesModel->setData( idx, m_toggled, Qt::CheckStateRole );
    }
}
