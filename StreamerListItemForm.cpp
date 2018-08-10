#include "StreamerListItemForm.h"
#include "ui_StreamerListItemForm.h"

#include <QMovie>

StreamerListItemForm::StreamerListItemForm(bool sort, QWidget* parent) : QWidget(parent)
{
    m_streamer = nullptr;

    m_sort = sort;
    m_parent = nullptr;
}

StreamerListItemForm::StreamerListItemForm(Streamer* streamer, StreamerListItemForm* parent) : QWidget(parent)
{
    ui = new Ui::StreamerListItemForm;
    ui->setupUi(this);
    ui->streamerName->setText(streamer->getName());

    m_parent = parent;

    m_streamer = streamer;
    QObject::connect(m_streamer, SIGNAL(availabilityChanged(bool)), SLOT(updateStatus()));
    QObject::connect(m_streamer, SIGNAL(recordingChanged(bool)), SLOT(updateStatus()));

    m_gif = new QMovie(":/resources/loading_spinner.gif");
    m_gif->setScaledSize(ui->streamerStatus->size());

    updateStatus();

    hide(); //hide widget because it's rendered by StreamerListDelegate
}

StreamerListItemForm::~StreamerListItemForm()
{
    //delete ui;
}

QVariant StreamerListItemForm::data(int column) const
{
    Q_UNUSED(column)

    return QVariant::fromValue(static_cast<void*>(const_cast<StreamerListItemForm*>(this)));
}

StreamerListItemForm* StreamerListItemForm::parent() const
{
    return m_parent;
}

int StreamerListItemForm::row() const
{
    if(m_parent)
    {
        return m_parent->m_childs.indexOf(const_cast<StreamerListItemForm*>(this));
    }
    else
    {
        return 0;
    }
}

StreamerListItemForm* StreamerListItemForm::child(int row) const
{
    return m_childs.value(row);
}

int StreamerListItemForm::columnCount() const
{
    return 1;
}

int StreamerListItemForm::rowCount() const
{
    return m_childs.count();
}

int StreamerListItemForm::indexOf(Streamer* streamer)
{
    int index = -1;

    for(quint16 i = 0; i < m_childs.size(); ++i)
    {
        if(m_childs.at(i)->streamer() == streamer)
        {
            index = i;
            break;
        }
    }

    return index;
}

void StreamerListItemForm::prependChild(StreamerListItemForm* item)
{
    m_childs.prepend(item);
    sort();
}

void StreamerListItemForm::appendChild(StreamerListItemForm* item)
{
    m_childs.append(item);
    sort();
}

void StreamerListItemForm::insertChildAt(StreamerListItemForm* item, int row)
{
    m_childs.insert(row, item);
    sort();
}

void StreamerListItemForm::removeFirstChild()
{
    if(!m_childs.isEmpty())
    {
        delete m_childs.takeFirst();
    }
}

void StreamerListItemForm::removeLastChild()
{
    if(!m_childs.isEmpty())
    {
        delete m_childs.takeLast();
    }
}

void StreamerListItemForm::removeChildAt(int row)
{
    if(row < m_childs.size())
    {
        delete m_childs.takeAt(row);
    }
}

void StreamerListItemForm::clear()
{
    qDeleteAll(m_childs);
    m_childs.clear();
}

Streamer* StreamerListItemForm::streamer() const
{
    return m_streamer;
}

void StreamerListItemForm::updateStatus()
{
    if(m_streamer->isRecording())
    {
        ui->streamerStatus->setMovie(m_gif);
        m_gif->start();
    }
    else
    {
        QString resource = m_streamer->isOnline() ? ":/resources/connected.png" : ":/resources/not_connected.png";

        QPixmap pixmap = QPixmap::fromImage(QImage(resource)).scaled(10,
                                                                     10,
                                                                     Qt::KeepAspectRatio,
                                                                     Qt::SmoothTransformation);
        if(!pixmap.isNull())
        {
            ui->streamerStatus->setPixmap(pixmap);
        }
        else
        {
            ui->streamerStatus->setText(m_streamer->isOnline() ? "Y" : "N");
        }

        m_gif->stop();
    }

    m_parent->sort();
}

void StreamerListItemForm::sort()
{
    if(m_sort)
    {
        std::sort(m_childs.begin(), m_childs.end(), [] (const StreamerListItemForm* streamerListItemForm1, const StreamerListItemForm* streamerListItemForm2) -> bool
        {
            if(streamerListItemForm1->streamer()->isOnline() != streamerListItemForm2->streamer()->isOnline())
            {
                return streamerListItemForm1->streamer()->isOnline() > streamerListItemForm2->streamer()->isOnline();
            }
            else
            {
                return streamerListItemForm1->streamer()->getName() < streamerListItemForm2->streamer()->getName();
            }
        });
    }
}
