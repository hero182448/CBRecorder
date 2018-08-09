#ifndef STREAMERLISTDELEGATE_H
#define STREAMERLISTDELEGATE_H

#include <QPainter>
#include <QStyledItemDelegate>

class StreamerListDelegate : public QStyledItemDelegate
{
    public:
        StreamerListDelegate(QObject* parent = nullptr);

        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        //virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // STREAMERLISTDELEGATE_H
