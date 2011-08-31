#ifndef RECENT_TRACKS_WIDGET_H_
#define RECENT_TRACKS_WIDGET_H_

#include <QWidget>

namespace lastfm { class Track; }
using lastfm::Track;

class RecentTracksWidget : public QWidget 
{
Q_OBJECT
public:
    RecentTracksWidget( QWidget* parent = 0 );

public slots:
    void onCurrentChanged( int index );

protected slots:
    void onTrackClicked( const Track& track );
    void onBackClicked();
    void onMoveFinished( class QLayoutItem* i );

protected:
    class SideBySideLayout* m_layout;
    class ScrobblesWidget* m_scrobbles;
};

#endif //RECENT_TRACKS_WIDGET_H_
