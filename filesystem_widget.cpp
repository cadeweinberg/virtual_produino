#include "filesystem_widget.h"

#include <QVBoxLayout>

FilesystemWidget::FilesystemWidget(QWidget *parent)
    : QWidget{parent}
    , m_filesystem_model(new QFileSystemModel(this))
    , m_filesystem_view(new QTreeView(this))
{
    const QString &root_path = QDir::homePath();
    m_filesystem_model->setRootPath(root_path);
    m_filesystem_model->setFilter(QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot);

    m_filesystem_view->setModel(m_filesystem_model);
    m_filesystem_view->setRootIndex(m_filesystem_model->index(root_path));
    m_filesystem_view->setAnimated(true);
    m_filesystem_view->setHeaderHidden(false);
    m_filesystem_view->setSortingEnabled(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_filesystem_view);
    setLayout(layout);
}

QString FilesystemWidget::getRootPath() const
{
    return m_filesystem_model->rootPath();
}

void FilesystemWidget::setRootPath(const QString &path)
{
    m_filesystem_model->setRootPath(path);
    const QModelIndex index = m_filesystem_model->index(path);
    if (index.isValid()) {
        m_filesystem_view->setRootIndex(index);
    }
}
