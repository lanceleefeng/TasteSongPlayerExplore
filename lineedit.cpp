#include "lineedit.h"

#include <QDebug>

LineEdit::LineEdit()
{
    
}

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
    //: QLineEdit(parent2)
    //: QWidget(parent)
    //: QWidget(parent)
{

}

void LineEdit::enterEvent(QEvent *event)
{
    this->selectAll();
    this->setFocus();
}

//void LineEdit::keyReleaseEvent(QKeyEvent *event)
//{
//    qDebug() << event->key();
//    QLineEdit::keyPressEvent(event);
//}

