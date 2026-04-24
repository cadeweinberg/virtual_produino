#include "image_selection.h"

#include <QIcon>
#include <QImageReader>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "settings.h"

ImageSelection::ImageSelection(QObject *parent)
    : QAbstractListModel(parent)
    , m_file_icon_provider()
    , m_files()
{
}

ImageSelection::ImageSelection(const QJsonObject &json, QObject *parent)
    : QAbstractListModel(parent)
    , m_file_icon_provider()
    , m_files()
{
    addJson(json);
}

QVariant ImageSelection::headerData(int section, Qt::Orientation orientation, int role)
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            if (section == 0){
                return QString("order");
            }

            if (section == 1) {
                return QString("file");
            }

            [[fallthrough]];
        default:
            return {};
        }
    }

    if (orientation == Qt::Vertical) {
        const Binding &binding = m_files.at(section);
        switch (role) {
        case Qt::DisplayRole:
            return QString("%1").arg(binding.order);
        default:
            return {};
        }
    }

    Q_UNREACHABLE();
    return {};
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
        return binding.info.fileName();
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
        if (!Settings::isImage(fi)) continue;
        toAdd.push_back(fi);
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow + toAdd.size() - 1);
    for (qint64 i = 0; i < toAdd.size(); ++i)
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

    qint64 order = m_files.isEmpty() ? 0 : m_files.back().order + 1;
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
    for (qint64 i = 0; i < m_files.size(); ++i)
        m_files[i].order = i;

    emit dataChanged(index(0,0), index(rowCount()-1,0), { OrderRole });
}

QJsonObject ImageSelection::Binding::toJSON() const
{
    QJsonObject json;
    json["order"] = order;
    json["path"]  = info.absoluteFilePath();
    return json;
}

std::optional<ImageSelection::Binding> ImageSelection::Binding::fromJSON(const QJsonObject &json)
{
    // #NOTE: I don't like this, we could end up inserting a bunch of
    // empty list object from malformed json. I would prefer inserting nothing.
    // if we don't see a valid filepath, we don't want to insert a binding.
    // the order on the added elements is almost immaterial, as the user can freely
    // reorder elements at will. If the filepath is incorrect however, that is
    // bad data for the rest of the system, so we should stop that before we go any
    // further. We are in a stack method, whose signature demands we construct a
    // Binding. So lets change the signature.

    qint64 order = 0;
    if (const QJsonValue v = json["order"]; v.isDouble()) {
        order = v.toInteger();
    }

    QString path;
    if (const QJsonValue v = json["path"]; v.isString()) {
        path = v.toString();
    }

    QFileInfo info(path);

    if (info.isFile()) {
        return {{order, info}};
    }

    return std::nullopt;
}

QJsonObject ImageSelection::toJSON() const
{
    const QList<Binding> bound = m_files;
    QJsonObject json;
    QJsonArray bindings;
    for (Binding const &binding : bound) {
        bindings.append(binding.toJSON());
    }
    json["bindings"] = bindings;
    return json;
}

void ImageSelection::addJson(const QJsonObject &json)
{
    if (QJsonValue v = json["bindings"]; v.isArray()) {
        QJsonArray array = v.toArray();
        QList<Binding> temp;
        for (auto &&value : array) {
            auto &&bound = Binding::fromJSON(value.toObject());
            if (bound) {
                temp.emplaceBack(bound.value());
            }
        }

        beginInsertRows(QModelIndex(), m_files.size(), m_files.size() + temp.size());
        for (auto &&binding : temp) {
            m_files.append(binding);
        }
        endInsertRows();
        renumber();
    }
}
