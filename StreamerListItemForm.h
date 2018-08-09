#ifndef STREAMERLISTITEMFORM_H
#define STREAMERLISTITEMFORM_H

#include <QWidget>

#include "Streamer.h"

namespace Ui {
    class StreamerListItemForm;
}

class StreamerListItemForm : public QWidget
{
        Q_OBJECT

    public:
        explicit StreamerListItemForm(bool sort = true, QWidget* parent = 0);
        explicit StreamerListItemForm(Streamer* streamer, StreamerListItemForm* parent = 0);
        ~StreamerListItemForm();

        QVariant data(int column) const;

        StreamerListItemForm* parent() const;
        int row() const;

        StreamerListItemForm* child(int row) const;
        int columnCount() const;
        int rowCount() const;

        int indexOf(Streamer* streamer);
        void prependChild(StreamerListItemForm* item);
        void appendChild(StreamerListItemForm* item);
        void insertChildAt(StreamerListItemForm* item, int row);
        void removeFirstChild();
        void removeChild(StreamerListItemForm* item);
        void removeChildAt(int row);
        void removeLastChild();
        void clear();

        Streamer* streamer() const;

    signals:
        void itemUpdated();

    private slots:
        void updateStatus();

    private:
        void sort();

    private:
        Ui::StreamerListItemForm *ui;
        QMovie* m_gif;

        bool m_sort;

        Streamer* m_streamer;

        StreamerListItemForm* m_parent;
        QList<StreamerListItemForm*> m_childs;
};

#endif // STREAMERLISTITEMFORM_H
