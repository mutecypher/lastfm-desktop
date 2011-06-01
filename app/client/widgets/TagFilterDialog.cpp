
#include <lastfm/XmlQuery>

#include "TagFilterDialog.h"
#include "ui_TagFilterDialog.h"

TagFilterDialog::TagFilterDialog( const lastfm::RadioStation& radioStation, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagFilterDialog)
{
    ui->setupUi(this);

    connect( ui->retune, SIGNAL(clicked()), SLOT(accept()) );

    connect( radioStation.getTagSuggestions( 20 ), SIGNAL(finished()), SLOT(onGotTagSuggestions()) );
}

TagFilterDialog::~TagFilterDialog()
{
    delete ui;
}

void
TagFilterDialog::onGotTagSuggestions()
{
    lastfm::XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    foreach ( const lastfm::XmlQuery suggestion, lfm.children("suggestion"))
        ui->tagComboBox->addItem( suggestion["tag"]["name"].text() );
}

QString
TagFilterDialog::tag() const
{
    return ui->tagComboBox->currentText();
}
