
#include <QLabel>
#include <QVBoxLayout>

#include "NothingPlayingWidget.h"

NothingPlayingWidget::NothingPlayingWidget( QWidget* parent )
    :QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->addWidget( new QLabel( tr("THERE'S NOTHING PLAYING!"), this ), Qt::AlignHCenter );
}
