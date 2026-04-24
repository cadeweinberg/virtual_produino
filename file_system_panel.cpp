#include "file_system_panel.h"

#include <QVBoxLayout>

#include "settings.h"

FileSystemPanel::FileSystemPanel(QWidget *parent)
    : QWidget{parent}
    , m_filter(new FileSystemImageFilter(this))
    , m_model(new QFileSystemModel(this))
    , m_view(new QTreeView(this))
{
    const QString &root_path = QDir::homePath();
    m_model->setRootPath(root_path);
    m_model->setFilter(QDir::Filter::AllEntries);
    m_model->setNameFilterDisables(true);

    m_filter->setSourceModel(m_model);
    m_filter->setFilterKeyColumn(0);
    m_filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filter->setFilterRegularExpression(Settings::getIsImageRegularExpression());

    m_view->setModel(m_filter);
    m_view->setRootIndex(m_filter->mapFromSource(m_model->index(root_path)));
    m_view->setAnimated(true);
    m_view->setHeaderHidden(false);
    m_view->setSortingEnabled(false);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setDragEnabled(true);
    m_view->setDragDropMode(QAbstractItemView::DragOnly);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    setLayout(layout);
}

QString FileSystemPanel::getRootPath() const
{
    return m_model->rootPath();
}

void FileSystemPanel::setRootPath(const QString &path)
{
    m_model->setRootPath(path);
    m_filter->setSourceModel(m_model);
    const QModelIndex index = m_filter->mapFromSource(m_model->index(path));
    if (index.isValid()) {
        m_view->setRootIndex(index);
    }
}

QFileInfo FileSystemPanel::fileInfo(const QModelIndex &index) {
    if (!index.isValid()) { return {}; }

    return m_model->fileInfo(m_filter->mapToSource(index));
}
