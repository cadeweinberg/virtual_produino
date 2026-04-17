#include "image_selection_panel.h"

#include <QVBoxLayout>
#include <QShortcut>

ImageSelectionPanel::ImageSelectionPanel(QWidget *parent)
    : QWidget{parent}
    , m_model(new ImageSelection(this))
    , m_view(new QListView(this))
{
    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setDragDropMode(QAbstractItemView::DragDrop);
    m_view->setDefaultDropAction(Qt::MoveAction);
    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDropIndicatorShown(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_view);
    setLayout(layout);

    connect(m_view, &QListView::doubleClicked,
            this, &ImageSelectionPanel::remove);


    auto *delShortcut = new QShortcut(QKeySequence::Delete, m_view);
    auto *backShortcut = new QShortcut(QKeySequence::Backspace, m_view);

    connect(delShortcut, &QShortcut::activated, this, &ImageSelectionPanel::removeSelection);
    connect(backShortcut, &QShortcut::activated, this, &ImageSelectionPanel::removeSelection);
}

void ImageSelectionPanel::remove(const QModelIndex &index)
{
    m_model->removeRow(index.row());
}

void ImageSelectionPanel::removeSelection()
{
    auto rows = m_view->selectionModel()->selectedRows();
    if (rows.isEmpty()) return;

    std::sort(rows.begin(), rows.end(),
              [](const QModelIndex &a, const QModelIndex &b){ return a.row() > b.row(); });

    for (const QModelIndex &idx : std::as_const(rows))
        m_model->removeRow(idx.row());

}
