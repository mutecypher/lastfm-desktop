#ifndef RECENT_TRACKS_WIDGET_H_
#define RECENT_TRACKS_WIDGET_H_

#include <QWidget>

namespace lastfm { class Track; }
using lastfm::Track;

class ScrobblesWidget : public QWidget
{
Q_OBJECT
public:
    ScrobblesWidget( QWidget* parent = 0 );

public slots:
    void onCurrentChanged( int index );
    void refresh();

protected slots:
    void onTrackClicked( class TrackWidget& trackWidget );
    void onBackClicked();
    void onMoveFinished( class QLayoutItem* i );
    void onMetadataWidgetFinished();

protected:
    class SideBySideLayout* m_layout;
    class ScrobblesListWidget* m_scrobbles;
};

#endif //RECENT_TRACKS_WIDGET_H_
