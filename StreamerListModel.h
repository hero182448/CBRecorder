#ifndef STREAMERLISTMODEL_H
#define STREAMERLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "Streamer.h"
#include "StreamerListItemForm.h"

class StreamerListModel : public QAbstractItemModel
{
        Q_OBJECT

    public:
        StreamerListModel(bool sort = true, QObject* parent = nullptr);

        QVariant headerData(int column, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QVariant data(const QModelIndex& index, int role) const;
        QModelIndex parent(const QModelIndex& index) const;
        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        Qt::ItemFlags flags(const QModelIndex& index) const;

        void propendItem(Streamer* streamer);
        void appendItem(Streamer* streamer);
        void insertItemAt(Streamer* streamer, int row);
        void removeFirstItem();
        void removeLastItem();
        void removeItem(Streamer* streamer);
        void removeItemAt(int row);
        void clear();

        StreamerListItemForm* rootItem();

    signals:
        void itemUpdated();

    private slots:
        void onStreamerDestroyed(QObject* streamer);

    private:
        StreamerListItemForm* m_rootItem;
};

#endif // STREAMERLISTMODEL_H
