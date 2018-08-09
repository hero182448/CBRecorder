#include "StreamerListModel.h"

StreamerListModel::StreamerListModel(bool sort, QObject* parent) : QAbstractItemModel(parent)
{
    m_rootItem = new StreamerListItemForm(sort, (QWidget*)parent);
}

QVariant StreamerListModel::headerData(int column, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->data(column);
    }
    else
    {
        return QVariant();
    }
}

QVariant StreamerListModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole)
    {
        return static_cast<StreamerListItemForm*>(index.internalPointer())->data(index.column());
    }
    else
    {
        return QVariant();
    }
}

QModelIndex StreamerListModel::parent(const QModelIndex& index) const
{
    StreamerListItemForm* parentItem = static_cast<StreamerListItemForm*>(index.internalPointer())->parent();

    if(index.isValid() && parentItem && parentItem != m_rootItem)
    {
        return createIndex(parentItem->row(), 0, parentItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex StreamerListModel::index(int row, int column, const QModelIndex& parent) const
{
    if(hasIndex(row, column, parent))
    {
        StreamerListItemForm* parentItem;

        if(parent.isValid())
        {
            parentItem = static_cast<StreamerListItemForm*>(parent.internalPointer());
        }
        else
        {
            parentItem = m_rootItem;
        }

        StreamerListItemForm* childItem = parentItem->child(row);

        if(childItem)
        {
            return createIndex(row, column, childItem);
        }
        else
        {
            return QModelIndex();
        }
    }
    else
    {
        return QModelIndex();
    }
}

int StreamerListModel::rowCount(const QModelIndex& parent) const
{
    StreamerListItemForm* parentItem;

    if(parent.isValid())
    {
        parentItem = static_cast<StreamerListItemForm*>(parent.internalPointer());
    }
    else
    {
        parentItem = m_rootItem;
    }

    return parentItem->rowCount();
}

int StreamerListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return static_cast<StreamerListItemForm*>(parent.internalPointer())->columnCount();
    }
    else
    {
        return m_rootItem->columnCount();
    }
}

Qt::ItemFlags StreamerListModel::flags(const QModelIndex &index) const
{
    if(index.isValid())
    {
        return QAbstractItemModel::flags(index);
    }
    else
    {
        return 0;
    }
}

void StreamerListModel::propendItem(Streamer* streamer)
{
    beginInsertRows(QModelIndex(), 0, 0);
    StreamerListItemForm* item = new StreamerListItemForm(streamer, m_rootItem);
    QObject::connect(item, SIGNAL(itemUpdated()), SIGNAL(itemUpdated()));
    m_rootItem->prependChild(item);
    endInsertRows();
}

void StreamerListModel::appendItem(Streamer* streamer)
{
    beginInsertRows(QModelIndex(), m_rootItem->rowCount(), m_rootItem->rowCount());
    StreamerListItemForm* item = new StreamerListItemForm(streamer, m_rootItem);
    QObject::connect(item, SIGNAL(itemUpdated()), SIGNAL(itemUpdated()));
    m_rootItem->appendChild(item);
    endInsertRows();

    QObject::connect(streamer, SIGNAL(destroyed(QObject*)), SLOT(onStreamerDestroyed(QObject*)));
}

void StreamerListModel::insertItemAt(Streamer* streamer, int row)
{
    if(row < m_rootItem->rowCount())
    {
        beginInsertRows(QModelIndex(), row, row);
        StreamerListItemForm* item = new StreamerListItemForm(streamer, m_rootItem);
        QObject::connect(item, SIGNAL(itemUpdated()), SIGNAL(itemUpdated()));
        m_rootItem->insertChildAt(item, row);
        endInsertRows();
    }
}

void StreamerListModel::removeFirstItem()
{
    if(m_rootItem->rowCount() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_rootItem->removeFirstChild();
        endRemoveRows();
    }
}

void StreamerListModel::removeLastItem()
{
    if(m_rootItem->rowCount() > 0)
    {
        beginRemoveRows(QModelIndex(), m_rootItem->rowCount() - 1, m_rootItem->rowCount() - 1);
        m_rootItem->removeLastChild();
        endRemoveRows();
    }
}

void StreamerListModel::removeItem(Streamer* streamer)
{
    if(m_rootItem->indexOf(streamer) != -1)
    {
        beginRemoveRows(QModelIndex(), m_rootItem->indexOf(streamer), m_rootItem->indexOf(streamer));
        m_rootItem->removeChildAt(m_rootItem->indexOf(streamer));
        endRemoveRows();
    }
}

void StreamerListModel::removeItemAt(int row)
{
    if(row < m_rootItem->rowCount())
    {
        beginRemoveRows(QModelIndex(), row, row);
        m_rootItem->removeChildAt(row);
        endRemoveRows();
    }
}

void StreamerListModel::onStreamerDestroyed(QObject* streamer)
{
    removeItem(static_cast<Streamer*>(streamer));
}

void StreamerListModel::clear()
{
    if(m_rootItem->rowCount() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_rootItem->rowCount() - 1);
        m_rootItem->clear();
        endRemoveRows();
    }
}

StreamerListItemForm* StreamerListModel::rootItem()
{
    return m_rootItem;
}
