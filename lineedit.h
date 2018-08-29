#ifndef LINE_EDIT_H
#define LINE_EDIT_H

#include <QLineEdit>
#include <QKeyEvent>


class LineEdit : public QLineEdit
{
//Q_OBJECT

public:
    LineEdit();
    LineEdit(QWidget *parent = Q_NULLPTR);

protected:
    void enterEvent(QEvent *event);
    //void keyReleaseEvent(QKeyEvent *event);


};

#endif // LINE_EDIT_H
