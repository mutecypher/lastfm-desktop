#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H

#include <QComboBox>

class ShortcutEdit : public QComboBox
{
    Q_OBJECT

public:
    ShortcutEdit( QWidget* parent = 0 );

    void setTextValue( QString str );
    void keyPressEvent( QKeyEvent* e );
    QString textValue() const;

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers( Qt::KeyboardModifiers modifiers );
    int key() const;
    void setKey( int key );

private:
    Qt::KeyboardModifiers m_modifiers;
    int m_key;
};

#endif // SHORTCUTEDIT_H
