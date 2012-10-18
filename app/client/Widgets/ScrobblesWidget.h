#ifndef RECENT_TRACKS_WIDGET_H_
#define RECENT_TRACKS_WIDGET_H_

#include <QWidget>

namespace lastfm { class Track; }
using lastfm::Track;

namespace Ui { class ScrobblesWidget; }

class ScrobblesWidget : public QWidget
{
Q_OBJECT
public:
    explicit ScrobblesWidget( QWidget* parent = 0 );
    ~ScrobblesWidget();

public slots:
    void onCurrentChanged( int index );
    void refresh();

protected slots:
    void onTrackClicked( class TrackWidget& trackWidget );
    void onBackClicked();
    void onMoveFinished( class QLayoutItem* i );
    void onMetadataWidgetFinished();

protected:
    Ui::ScrobblesWidget* ui;
    int m_lastIndex;
};

#endif //RECENT_TRACKS_WIDGET_H_
