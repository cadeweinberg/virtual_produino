#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <memory>

#include <QMainWindow>
#include <QUdpSocket>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QTreeView>
#include <QTableView>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // UI
    void onNew();
    void onOpen();
    void onOpenDirectory();
    void onFileViewItemClicked(const QModelIndex &index);
    void onSelectionChanged(const QModelIndex &selected, const QModelIndex &deselected);

    // Filesystem
    void updateDirectoryListing();
    void previewImage(const QString &path);

    // Frame Associatations
    void populateTable();

    // Networking
    void readPendingDatagrams();

private:
    void sendJson(const QJsonObject& obj);
    void receiveEvent(const QJsonObject& obj);

    void setupUI();
    void setupFilesystem();
    void setupNetworking();

    // UI
    std::unique_ptr<QSplitter>  m_splitter;
    std::unique_ptr<QTreeView>  m_tree_view;
    std::unique_ptr<QTableView> m_table_view;
    std::unique_ptr<QLabel>     m_preview;

    std::unique_ptr<QItemSelectionModel> m_item_selection_model;
    std::unique_ptr<QFileSystemModel>    m_filesystem_model;

    // Filesystem
    QString m_root_path;

    // Networking
    QUdpSocket     m_socket;
    QHostAddress   m_df_address;
    qint16         m_df_port;
    qint64         m_df_frame;
};
#endif // MAIN_WINDOW_H
