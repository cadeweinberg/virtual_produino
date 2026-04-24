#include "file_system_image_filter.h"

#include <QFileSystemModel>

FileSystemImageFilter::FileSystemImageFilter(QObject *parent)
    : QSortFilterProxyModel{parent}
{}


bool FileSystemImageFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {

    auto *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fsm) return Parent::filterAcceptsRow(sourceRow, sourceParent);

    const QModelIndex idx = fsm->index(sourceRow, 0, sourceParent);
    if (!idx.isValid()) return false;

    if (fsm->isDir(idx)) return true; // keep directories visible

    // apply the regex to the file name column (0)
    return Parent::filterAcceptsRow(sourceRow, sourceParent);

}