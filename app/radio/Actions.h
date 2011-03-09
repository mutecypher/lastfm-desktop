#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>

class Actions : public QObject
{
    Q_OBJECT

public:
    Actions();

    void doConnect( QObject* object );

    class QAction* m_loveAction;
    class QAction* m_banAction;
    class QAction* m_playAction;
    class QAction* m_skipAction;
};

#endif // ACTIONS_H
