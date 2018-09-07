#ifndef LINE_EDIT_H
#define LINE_EDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QMouseEvent>


class LineEdit : public QLineEdit
{
//Q_OBJECT

public:
    LineEdit();
    LineEdit(QWidget *parent = Q_NULLPTR);

protected:
    void enterEvent(QEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
    //void keyReleaseEvent(QKeyEvent *event);

};

#endif // LINE_EDIT_H
