
#ifndef MEDIA_KEY_H
#define MEDIA_KEY_H

#include <QApplication>
#include <QObject>

class MediaKey : public QObject
{
    Q_OBJECT
public:
    explicit MediaKey( QObject* parent );

    bool macEventFilter( EventHandlerCallRef, EventRef event );

private slots:
    void initialize();
    void applicationDidFinishLaunching( void* aNotification );
};

#endif // MEDIA_KEY_H
