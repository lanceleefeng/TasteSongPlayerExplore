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
    //setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::ClickFocus);
}


void LineEdit::enterEvent(QEvent *event)
{
    this->selectAll();
    this->setFocus();
}


// 使用鼠标释放来选择，后果是不能用鼠标定位要修改的了，因为变成了选择..
//void LineEdit::mouseReleaseEvent(QMouseEvent *event)
//{
//    this->selectAll();
//    this->setFocus();
//}

//void LineEdit::keyReleaseEvent(QKeyEvent *event)
//{
//    qDebug() << event->key();
//    QLineEdit::keyPressEvent(event);
//}

