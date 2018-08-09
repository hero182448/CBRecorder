#ifndef STREAMERLISTVIEW_H
#define STREAMERLISTVIEW_H

#include <QListView>

#include "Streamer.h"
#include "StreamerListDelegate.h"
#include "StreamerListModel.h"

class StreamerListView : public QListView
{
        Q_OBJECT

    public:
        StreamerListView(bool sort = true, QWidget* parent = nullptr);

        StreamerListModel* streamersListModel() const;
        Streamer* getSelectedStreamer();

        int rowCount() const;
        int columnCount() const;
        void propendItem(Streamer* streamer);
        void insertItemAt(Streamer* streamer, int row);
        void removeFirstItem();
        void removeLastItem();
        void removeItemAt(int row);
        void clear();

        QSize maxSize();

    public slots:
        void appendItem(Streamer* streamer);
        void removeItem(Streamer* streamer);

    signals:
        void streamerClicked(Streamer* streamer);
        void streamerEntered(Streamer* streamer);
        void streamerPressed(Streamer* streamer);
        void streamerSelected(Streamer* streamer);
        void streamerDoubleClicked(Streamer* streamer);

    private slots:
        void onStreamerAdded(Streamer* streamer);
        void onStreamerRemoved(Streamer* streamer);

        void onContextMenuRequested(QPoint position);
        void onItemClicked(const QModelIndex& index);
        void onItemEntered(const QModelIndex& index);
        void onItemPressed(const QModelIndex& index);
        void onItemDoubleClicked(const QModelIndex& index);
        void onCurrentRowChanged(QModelIndex current, QModelIndex previous);

    private:
        StreamerListModel* m_streamerListModel;
        StreamerListDelegate* m_streamerListDelegate;

        QTimer* m_updateGifTimer;
};

#endif // STREAMERLISTVIEW_H
