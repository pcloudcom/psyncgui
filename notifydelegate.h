#ifndef NOTIFYDELEGATE_H
#define NOTIFYDELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QColor>
class NotifyDelegate : public QStyledItemDelegate
{
public:
    NotifyDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setNumNew(quint32 newcnt);
protected:
    //virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
    int minColumnHeight;
    qreal textDocWidth;
    quint32 numNew;
    QColor mouseOverColor, redNtfColor, newNtfColor, separatorColor;
};

#endif // NOTIFYDELEGATE_H
