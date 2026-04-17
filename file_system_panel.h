#ifndef FILE_SYSTEM_PANEL_H
#define FILE_SYSTEM_PANEL_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>

class FileSystemPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemPanel(QWidget *parent = nullptr);

    QFileSystemModel *model() const { return m_model; }
    QTreeView *view() const { return m_view; }
    QItemSelectionModel *selectionModel() const { return m_view->selectionModel(); }
    QString getRootPath() const;
    void setRootPath(const QString &path);

private:
    void select(const QModelIndex &index);

    QFileSystemModel *m_model;
    QTreeView        *m_view;

signals:
};

#endif // FILE_SYSTEM_PANEL_H
