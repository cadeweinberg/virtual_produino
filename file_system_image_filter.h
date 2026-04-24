#ifndef FILE_SYSTEM_IMAGE_FILTER_H
#define FILE_SYSTEM_IMAGE_FILTER_H

#include <QObject>
#include <QSortFilterProxyModel>

class FileSystemImageFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    using Parent = QSortFilterProxyModel;

    explicit FileSystemImageFilter(QObject *parent = nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
signals:
};

#endif // FILE_SYSTEM_IMAGE_FILTER_H
