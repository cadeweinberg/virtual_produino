#ifndef IMAGE_SELECTION_H
#define IMAGE_SELECTION_H

#include <optional>

#include <QAbstractListModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QVector>

class ImageSelection : public QAbstractListModel
{
    Q_OBJECT
public:
    struct Binding {
        qint64 order;
        QFileInfo info;

        Binding(qint64 order, const QFileInfo& info)
            : order(order), info(info) {}

        QJsonObject toJSON() const;
        static std::optional<Binding> fromJSON(const QJsonObject &json);
    };

    enum Roles {
        OrderRole = Qt::UserRole + 1,
        PathRole
    };

    explicit ImageSelection(QObject * parent = nullptr);
    explicit ImageSelection(const QJsonObject &json, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool moveRows(const QModelIndex &srcParent, int srcRow, int count,
                  const QModelIndex &dstParent, int dstChild) override;

    Qt::DropActions supportedDragActions() const override {
        return Qt::MoveAction;
    }

    Qt::DropActions supportedDropActions() const override {
        return Qt::MoveAction | Qt::CopyAction;
    }

    QStringList mimeTypes() const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    void removeByPaths(const QStringList &paths);

    bool contains(const QString&absolute_path) const;
    void addFile(const QFileInfo &info);
    void addFiles(const QVector<QFileInfo> &files);
    const QVector<Binding> &files() const { return m_files; }

    QJsonObject toJSON() const;
    void addJson(const QJsonObject &json);

private:
    void renumber();

    QFileIconProvider m_file_icon_provider;
    QVector<Binding>  m_files;
};

#endif // IMAGE_SELECTION_H
