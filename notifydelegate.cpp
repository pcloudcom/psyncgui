#include "notifydelegate.h"
#include <QLabel>
#include <QDebug>
//#include <QApplication>
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QAbstractItemView>
#include <QTextOption>
#include <QMouseEvent>


NotifyDelegate::NotifyDelegate(QObject *parent) //++ numRed
    : QStyledItemDelegate(parent)
{
    qDebug()<<"NotifyDelegate create";
    //++ calc size hint width - min(desktop/6 150)
    //iconmargin
    minColumnHeight = 60;
    numNew = 0;
    separatorColor = "#EEEEEE";
    mouseOverColor = "#F3FBFE";
    redNtfColor = "#F4F4F4";

}

void NotifyDelegate::setNumNew(quint32 newcnt)
{
    this->numNew = newcnt;
}

void NotifyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        QStyledItemDelegate::paint(painter, option, index);

    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);
    //qDebug()<<"paint"<<index.column() << options.state;

    if(index.column() == 0 )
    {
        painter->save();

        QPixmap pixmap = index.data(Qt::EditRole).value<QPixmap>();
        QRect rect = options.rect;
        rect.setSize(QSize(options.rect.width()-6, options.rect.height() - 6));
        painter->drawPixmap(rect, pixmap, rect);

        QPen bottomLine;
        bottomLine.setColor(separatorColor);
        bottomLine.setWidth(1);
        painter->setPen(bottomLine);

        //  if(option.state & QStyle::State_Selected)
        //    painter->fillRect(option.rect, option.palette.color(QPalette::Background));

        bool hovered = false;
        if(option.state & QStyle::State_MouseOver)
        {
            hovered = true;
            QBrush brush(mouseOverColor);
            painter->fillRect(option.rect, brush);
        }
        else
        {
            QAbstractItemView *table = qobject_cast<QAbstractItemView *>(this->parent());
            if(table)
            {
                QModelIndex hoveredIndex = table->indexAt(table->viewport()->mapFromGlobal(QCursor::pos()));
                if(hoveredIndex.row() == index.row())
                {
                    hovered = true;
                    QBrush brush(mouseOverColor);
                    painter->fillRect(option.rect, brush);
                    table->update(hoveredIndex);
                }
            }
        }

        painter->drawLine(options.rect.bottomLeft(), options.rect.bottomRight());
        painter->restore();
    }
    else if(index.column() == 1)
    {
        painter->save();
        QTextDocument doc;
        doc.setDefaultTextOption(QTextOption(Qt::AlignVCenter));
        doc.setHtml(options.text);
        doc.setPageSize(QSize(options.rect.size()));
        // !!!doc.setDocumentMargin(12.0);
        options.text = "";

        QSize size = doc.size().toSize();
        size.setHeight(72);
        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0,0, options.rect.width(), options.rect.height());

        QPen bottomLine;
        bottomLine.setColor(separatorColor);
        bottomLine.setWidth(1);
        painter->setPen(bottomLine);

        // if(option.state & QStyle::State_Selected)
        //   painter->fillRect(clip, option.palette.color(QPalette::Background));

        bool hovered;
        if(option.state & QStyle::State_MouseOver )
        {
            QBrush brush(mouseOverColor);
            painter->fillRect(clip, brush);
        }
        else
        {
            QAbstractItemView *table = qobject_cast<QAbstractItemView *>(this->parent());
            if(table)
            {
                QModelIndex hoveredIndex = table->indexAt(table->viewport()->mapFromGlobal(QCursor::pos()));
                if(hoveredIndex.row() == index.row())
                {
                    hovered = true;
                    QBrush brush(mouseOverColor);
                    painter->fillRect(clip, brush);
                    table->update(hoveredIndex);
                }
            }
        }

        doc.drawContents(painter, clip);
        painter->drawLine(QPoint(0,options.rect.height()-1),QPoint(options.rect.width(),options.rect.height()-1));
        painter->restore();
    }
    //qDebug()<<"paint options"<<option.rect.size() << option.rect.bottomLeft() << options.rect.size() << options.rect.bottomLeft();
}


QSize NotifyDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //qDebug()<<"sizeHint";
    if (!index.isValid())
        return QStyledItemDelegate::sizeHint(option, index);

    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    if (index.column() == 0)
    {
        return QSize(60,72);
    }
    else
    {
        QTextDocument doc;
        // QTextOption opt(Qt::AlignVCenter);
        //opt.setWrapMode(QTextOption::WordWrap);
        doc.setDefaultTextOption(QTextOption(Qt::AlignVCenter));
        //doc.setDefaultTextOption(opt);
        doc.setHtml(options.text);
        doc.setTextWidth(320.0);
        qDebug()<<"sizeHint 1"<< index.column()<< index.row()<< doc.size().height() <<option.rect.height() <<doc.size().width();
        return QSize(doc.size().width(), ((minColumnHeight>  doc.size().height()) ? minColumnHeight :  doc.size().height()));
        //return QSize(, ((minColumnHeight>  doc.size().height()) ? minColumnHeight :  doc.size().height()));
    }
}

QWidget *NotifyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    //qDebug()<<"createEditor";
    if (index.column() == 0)
    {
        QLabel *iconLabel = new QLabel(parent);
        iconLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        iconLabel->setMargin(10);
        iconLabel->setMouseTracking(true);
        return iconLabel;
    }
    else if (index.column() == 1)
    {
        QLabel *textLabel = new QLabel(parent);
        //textLabel->setMargin(12);
        textLabel->setMouseTracking(true);
        return textLabel;
    }
    else
        return QStyledItemDelegate::createEditor(parent, option, index);

}

void NotifyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //qDebug()<<"setEditorData"<<index.model()->data(index, Qt::DisplayRole).toString();

    if (index.column() == 0 )
    {
        QString path  = index.model()->data(index, Qt::EditRole).toString();
        QLabel *iconLabel = static_cast<QLabel *>(editor); //++ setimage
        iconLabel->setPixmap(QPixmap(path));
        //qDebug()<<"setEditorData2.1"<<index.model()->data(index, Qt::DisplayRole).toString();
    }
    else if(index.column() == 1)
    {
        QLabel *textLabel = qobject_cast<QLabel *>(editor);
        textLabel->setTextFormat(Qt::RichText);
        textLabel->clear();
        //qDebug()<<"setEditorData2.2"<<index.model()->data(index, Qt::DisplayRole).toString();
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}

void NotifyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //qDebug()<< "setModelData"<<index;
    if(index.column() == 1)
    {
        QLabel *label = static_cast<QLabel*>(editor);
        model->setData(index,label->text(),Qt::EditRole);
        // qDebug()<< "setModelData" << model->data(index,Qt::DisplayRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}


void NotifyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
    //  qDebug()<<"updateEditorGeometry"<<index;
    editor->setGeometry(option.rect);
}
/*
bool NotifyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug()<<"editorEvent"<<index;

    return QStyledItemDelegate::editorEvent(event,model,option,index);
}
*/
