#include "StreamerListDelegate.h"

#include "StreamerListItemForm.h"
#include "MainWindow.h"
#include <qdebug.h>
StreamerListDelegate::StreamerListDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QWidget* StreamerListDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);

    return nullptr;
}

void StreamerListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    StreamerListItemForm* streamerListItem = static_cast<StreamerListItemForm*>(index.data().value<void*>());

    //Draw highlight before translating the painter
    if(option.state & QStyle::State_Selected)
    {
        QRect rect = option.rect;
        painter->fillRect(rect, QColor(0, 0, 0, 75));
    }

    painter->save();
    painter->translate(option.rect.topLeft());

    streamerListItem->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}
