#include "listsearchdialog.h"

#include <QShortcut>
#include <QKeySequence>

#include <QDebug>


ListSearchDialog::ListSearchDialog(QWidget *parent)
    : QDialog(parent)
    , listSearchListWidget(0)
{


    // Return是主键盘的回车键，Enter是小键盘的Enter
    QShortcut *returnShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter), this);

    connect(returnShortcut, SIGNAL(activated()), this, SLOT(playCurrent()));
    connect(enterShortcut, SIGNAL(activated()), this, SLOT(playCurrent()));

    // 不需要加Ctrl组合快捷键..
    //QListWidget自带了快捷键，就是上、下方向键..
    //同时还自带了PageUp、PageDown翻页..

    QShortcut *nextShortcut1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this);
    QShortcut *prevShortcut1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this);

    QShortcut *nextShortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this);
    QShortcut *prevShortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this);

    QShortcut *nextShortcut3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this);
    QShortcut *prevShortcut3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this);

    connect(nextShortcut1, SIGNAL(activated()), this, SLOT(next()));
    connect(nextShortcut2, SIGNAL(activated()), this, SLOT(next()));
    connect(nextShortcut3, SIGNAL(activated()), this, SLOT(next()));
    connect(prevShortcut1, SIGNAL(activated()), this, SLOT(prev()));
    connect(prevShortcut2, SIGNAL(activated()), this, SLOT(prev()));
    connect(prevShortcut3, SIGNAL(activated()), this, SLOT(prev()));

}

void ListSearchDialog::playCurrent() const
{
    QString text = listSearchListWidget->currentItem()->text();
    qDebug() << "In " << __FUNCTION__ << ": " << text;
    emit(playItem(text));
}

void ListSearchDialog::next() const
{
    int num = listSearchListWidget->count();
    int current = listSearchListWidget->currentRow();

    int nextRow = current >= num - 1 ? 0 : current + 1;
    listSearchListWidget->setCurrentRow(nextRow);
}

void ListSearchDialog::prev() const
{
    int num = listSearchListWidget->count();
    int current = listSearchListWidget->currentRow();

    int nextRow = current == 0 ? num - 1 : current -1;
    listSearchListWidget->setCurrentRow(nextRow);
}



void ListSearchDialog::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        qDebug() << "listSearchListWidget->count(): " << listSearchListWidget->count();
        qDebug() << "listSearchListWidget->item(0): " << listSearchListWidget->item(0);
        //if(listSearchListWidget->count() > 0) emit(playItem(listSearchListWidget->item(0)));
        //if(listSearchListWidget->count() > 0) emit(playItem(listSearchListWidget->item(0)->text()));


    }
}

//void ListSearchDialog::focusOutEvent(QFocusEvent *event)
//{
//    // 对于窗口无效，或者需要有响应键盘操作才有效..
//    qDebug() << __FUNCTION__;
//    //this->hide();
//}
