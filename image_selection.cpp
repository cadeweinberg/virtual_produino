#include "image_selection.h"

#include <QIcon>
#include <QImageReader>
#include <QMimeData>
#include <QUrl>

ImageSelection::ImageSelection(QObject *parent)
    : QAbstractListModel(parent)
    , m_supported_image_extensions()
    , m_file_icon_provider()
    , m_files()
{
    // #TODO supported image extensions is a candidate to place into
    // a global configuration/preferences object
    for (auto &&extension : QImageReader::supportedImageFormats()) {
        m_supported_image_extensions.insert(QString::fromLatin1(extension).toLower());
    }
}

int ImageSelection::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_files.size();
}

QVariant ImageSelection::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_files.size()) {
        return {};
    }

    const Binding &binding = m_files.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return QString("[%1] %2").arg(binding.order).arg(binding.info.fileName());
    case Qt::DecorationRole:
        return m_file_icon_provider.icon(binding.info);
    case OrderRole:
        return binding.order;
    case PathRole:
        return binding.info.absoluteFilePath();
    default:
        return {};
    }
}

bool ImageSelection::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row + count > m_files.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_files.removeAt(row);
    endRemoveRows();
    renumber();
    return true;
}

Qt::ItemFlags ImageSelection::flags(const QModelIndex &index) const
{
    auto f = QAbstractListModel::flags(index);
    if (index.isValid())
        f |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
    else
        f |= Qt::ItemIsDropEnabled;
    return f;
}

bool ImageSelection::moveRows(const QModelIndex &srcParent, int srcRow, int count,
              const QModelIndex &dstParent, int dstChild)
{
    if (srcParent.isValid() || dstParent.isValid())
        return false;

    if (srcRow < 0 || count <= 0 || srcRow + count > m_files.size())
        return false;

    if (dstChild > m_files.size())
        dstChild = m_files.size();

    if (dstChild >= srcRow && dstChild <= srcRow + count)
        return false;

    beginMoveRows(srcParent, srcRow, srcRow + count - 1, dstParent, dstChild);

    QVector<Binding> moving;
    for (int i = 0; i < count; ++i)
        moving.push_back(m_files.takeAt(srcRow));

    int insertPos = dstChild;
    if (dstChild > srcRow)
        insertPos -= count;

    for (int i = 0; i < moving.size(); ++i)
        m_files.insert(insertPos + i, moving[i]);

    endMoveRows();

    renumber();
    return true;
}

QStringList ImageSelection::mimeTypes() const
{
    return { QStringLiteral("text/uri-list") };
}

bool ImageSelection::dropMimeData(const QMimeData *data, Qt::DropAction action,
                  int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction) return true;
    if (!data->hasUrls()) return false;

    int insertRow = row;
    if (insertRow < 0) {
        // drop on empty area or on parent
        insertRow = parent.isValid() ? parent.row() : m_files.size();
        if (insertRow < 0) insertRow = m_files.size();
    }

    QVector<QFileInfo> toAdd;
    const auto urls = data->urls();
    for (const QUrl &url : urls) {
        if (!url.isLocalFile()) continue;
        QFileInfo fi(url.toLocalFile());
        if (!fi.isFile()) continue;
        if (!isImage(fi)) continue;
        toAdd.push_back(fi);
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow + toAdd.size() - 1);
    for (quint64 i = 0; i < toAdd.size(); ++i)
        m_files.emplace(insertRow + i, 0, toAdd[i]);
    endInsertRows();

    renumber();
    return true;
}

QMimeData *ImageSelection::mimeData(const QModelIndexList &indexes) const
{
    auto *mime = new QMimeData;
    QList<QUrl> urls;

    QSet<int> rows;
    for (const QModelIndex &idx : indexes)
        rows.insert(idx.row());

    for (int row : rows) {
        if (row < 0 || row >= m_files.size()) continue;
        urls.append(QUrl::fromLocalFile(m_files.at(row).info.absoluteFilePath()));
    }

    mime->setUrls(urls);
    return mime;
}

void ImageSelection::removeByPaths(const QStringList &paths)
{
    if (paths.isEmpty()) return;

    // remove from end → start
    for (int i = m_files.size() - 1; i >= 0; --i) {
        const QString path = m_files.at(i).info.absoluteFilePath();
        if (!paths.contains(path)) continue;
        removeRow(i);
    }
}

bool ImageSelection::isImage(const QFileInfo &info) const
{
    return m_supported_image_extensions.contains(info.suffix().toLower());
}

bool ImageSelection::contains(const QString &absolute_path) const
{
    auto it = std::find_if(m_files.constBegin(), m_files.constEnd(), [&absolute_path](const Binding &binding){
        return absolute_path == binding.info.absoluteFilePath();
    });

    return it != m_files.constEnd();
}

void ImageSelection::addFile(const QFileInfo &info)
{

}

void ImageSelection::addFiles(const QVector<QFileInfo> &files)
{
    if (files.isEmpty()) { return; }

    quint64 order = m_files.isEmpty() ? 0 : m_files.back().order + 1;
    const int start = m_files.size();
    beginInsertRows(QModelIndex(), start, start + files.size() - 1);
    for (auto &&file : files) {
        m_files.emplace_back(order++, file);
    }
    endInsertRows();
    renumber();
}

void ImageSelection::renumber()
{
    for (quint64 i = 0; i < m_files.size(); ++i)
        m_files[i].order = i;

    emit dataChanged(index(0,0), index(rowCount()-1,0), { OrderRole });
}
