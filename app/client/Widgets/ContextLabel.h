#ifndef CONTEXTLABEL_H
#define CONTEXTLABEL_H

#include "lib/unicorn/widgets/Label.h"

class ContextLabel : public unicorn::Label
{
    Q_OBJECT
public:
    explicit ContextLabel( QWidget *parent = 0);

private:
    void paintEvent( QPaintEvent* event );

};

#endif // CONTEXTLABEL_H
