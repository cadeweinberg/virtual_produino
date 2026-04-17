#ifndef FILESYSTEM_WIDGET_H
#define FILESYSTEM_WIDGET_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>

class FilesystemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilesystemWidget(QWidget *parent = nullptr);

    QString getRootPath() const;
    void setRootPath(const QString &path);

private:
    QFileSystemModel *m_filesystem_model;
    QTreeView        *m_filesystem_view;

signals:
};

#endif // FILESYSTEM_WIDGET_H
