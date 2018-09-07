#ifndef LISTSEARCHDIALOG_H
#define LISTSEARCHDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QListWidget>
#include <QListWidgetItem>

class ListSearchDialog : public QDialog
{
    Q_OBJECT
public:
    ListSearchDialog(QWidget *parent = 0);
    QListWidget *listSearchListWidget; // 移到listSearchDialog中

    // 加signals要运行qmake
signals:

    // 跨对象传递时，用对象好像有问题..
    void playItem(const QListWidgetItem *item) const;
    void playItem(const QString text) const;
    //void playItem(QListWidgetItem *);

protected:

    void keyReleaseEvent(QKeyEvent *event);
    //void focusOutEvent(QFocusEvent *event);

private slots:

    void playCurrent() const;
    void next() const;
    void prev() const;




};

#endif // LISTSEARCHDIALOG_H