#include "StreamerListView.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMenu>
#include <QTimer>

#include "StreamerManager.h"

StreamerListView::StreamerListView(bool sort, QWidget* parent) : QListView(parent)
{
    m_streamerListModel = new StreamerListModel(sort, parent);
    setModel(m_streamerListModel);
    QObject::connect(m_streamerListModel, &StreamerListModel::itemUpdated, this, QOverload<>::of(&StreamerListView::repaint));

    m_streamerListDelegate = new StreamerListDelegate(this);
    setItemDelegate(m_streamerListDelegate);

    //setGridSize(QSize(100, 30));
    //setMovement(QListView::Static);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    //setSelectionMode(QAbstractItemView::ExtendedSelection); uncomment to enable multi selection
    //setUniformItemSizes(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &StreamerListView::customContextMenuRequested, this, &StreamerListView::onContextMenuRequested);

    setStyleSheet(QString("QListView {"
                          "background: transparent;"
                          "border: 0px;"
                          "}"));

    //    verticalScrollBar()->setStyle(new QCommonStyle());
    //    verticalScrollBar()->setStyleSheet(GuiUtils::SCROLL_BAR_STYLE);


    QObject::connect(this, &StreamerListView::clicked, this, &StreamerListView::onItemClicked);
    QObject::connect(this, &StreamerListView::pressed, this, &StreamerListView::onItemPressed);
    QObject::connect(this, &StreamerListView::doubleClicked, this, &StreamerListView::onItemDoubleClicked);

    QObject::connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &StreamerListView::onCurrentRowChanged);

    QObject::connect(StreamerManager::getInstance(), &StreamerManager::streamerAdded, this, &StreamerListView::onStreamerAdded);
    QObject::connect(StreamerManager::getInstance(), &StreamerManager::streamerDeleted, this, &StreamerListView::onStreamerRemoved);

    StreamerManager::getInstance()->initialize();

    m_updateGifTimer = new QTimer(this);
    m_updateGifTimer->start(0);
    QObject::connect(m_updateGifTimer, &QTimer::timeout, [=] ()
    {
        viewport()->repaint();
    });
}

StreamerListModel* StreamerListView::streamersListModel() const
{
    return m_streamerListModel;
}

Streamer* StreamerListView::getSelectedStreamer()
{
    QModelIndexList selection = selectionModel()->selectedRows();
    if(!selection.isEmpty())
    {
        return m_streamerListModel->rootItem()->child(selection.takeLast().row())->streamer();
    }

    return nullptr;
}

int StreamerListView::rowCount() const
{
    return m_streamerListModel->rowCount();
}

int StreamerListView::columnCount() const
{
    return m_streamerListModel->columnCount();
}

void StreamerListView::propendItem(Streamer* streamer)
{
    m_streamerListModel->propendItem(streamer);
}

void StreamerListView::appendItem(Streamer* streamer)
{
    m_streamerListModel->appendItem(streamer);
}

void StreamerListView::removeItem(Streamer* streamer)
{
    m_streamerListModel->removeItem(streamer);
}

void StreamerListView::onStreamerAdded(Streamer* streamer)
{
    appendItem(streamer);
}

void StreamerListView::onStreamerRemoved(Streamer* streamer)
{
    removeItem(streamer);
}

void StreamerListView::insertItemAt(Streamer* streamer, int row)
{
    m_streamerListModel->insertItemAt(streamer, row);
}

void StreamerListView::removeFirstItem()
{
    m_streamerListModel->removeFirstItem();
}

void StreamerListView::removeLastItem()
{
    m_streamerListModel->removeLastItem();
}

void StreamerListView::removeItemAt(int row)
{
    m_streamerListModel->removeItemAt(row);
}

void StreamerListView::clear()
{
    m_streamerListModel->clear();
}

QSize StreamerListView::maxSize()
{
    QSize size;

    for(quint16 i = 0; i < m_streamerListModel->rootItem()->rowCount(); ++i)
    {
        StreamerListItemForm* item = m_streamerListModel->rootItem()->child(i);

        if(item->size().width() > size.width())
        {
            size.setWidth(item->size().width());
        }

        if(item->size().height() > size.height())
        {
            size.setHeight(item->size().height());
        }
    }

    return size;
}

void StreamerListView::onContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    Streamer* selectedStreamer = nullptr;
    QModelIndexList selection = selectionModel()->selectedRows();
    while(!selection.isEmpty())
    {
        selectedStreamer = m_streamerListModel->rootItem()->child(selection.takeLast().row())->streamer();
        break;
    }

    if(selectedStreamer != nullptr)
    {
        QAction* copyURL = menu.addAction("Copy URL");
        QAction* record = menu.addAction(!selectedStreamer->isRecording() ? "Record" : "Stop recording");
        QAction* recordAsap = menu.addAction(!selectedStreamer->isRecordASAP() ? "Record ASAP" : "Not record ASAP");
        QAction* removeStreamer = menu.addAction("Remove streamer");

        QAction* selectedAction = menu.exec(viewport()->mapToGlobal(position));

        if(selectedAction == copyURL)
        {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(selectedStreamer->getM3u8());
        }
        else if(selectedAction == record && selectedStreamer->isOnline())
        {
            if(!selectedStreamer->isRecording())
            {
                selectedStreamer->startRecording();
            }
            else
            {
                selectedStreamer->stopRecording();
            }
        }
        else if(selectedAction == recordAsap)
        {
            selectedStreamer->setRecordASAP(!selectedStreamer->isRecordASAP());
            StreamerManager::getInstance()->updateRecordASAP();
        }
        else if(selectedAction == removeStreamer)
        {
            StreamerManager::getInstance()->deleteStreamer(selectedStreamer);
        }
    }
    else
    {
        qDebug() << "No streamer selected";
    }
}

void StreamerListView::onItemClicked(const QModelIndex& index)
{
    emit streamerClicked(m_streamerListModel->rootItem()->child(index.row())->streamer());
}

void StreamerListView::onItemEntered(const QModelIndex& index)
{
    emit streamerEntered(m_streamerListModel->rootItem()->child(index.row())->streamer());
}

void StreamerListView::onItemPressed(const QModelIndex& index)
{
    emit streamerPressed(m_streamerListModel->rootItem()->child(index.row())->streamer());
}

void StreamerListView::onItemDoubleClicked(const QModelIndex& index)
{
    emit streamerDoubleClicked(m_streamerListModel->rootItem()->child(index.row())->streamer());
}

void StreamerListView::onCurrentRowChanged(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous);

    if(m_streamerListModel->rootItem()->child(current.row()))
    {
        emit streamerSelected(m_streamerListModel->rootItem()->child(current.row())->streamer());
    }
}
