#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <memory>

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QTreeView>
#include <QTableView>
#include <QLabel>
#include <QImage>
#include <QList>

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
    void previewImage();

    // Networking
    void readPendingDatagrams();

    // Application Logic
    void populateImageSamples();
    void populateTable();
    bool writeFrame();
    QImage const &currentImageSample();
    static QImage sampleImage(QImage const &image);

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
    QUdpSocket     m_df_socket;
    QHostAddress   m_df_address;
    quint16        m_df_port;
    qint64         m_df_frame;
    QTcpSocket     m_arduino_socket;
    QHostAddress   m_arduino_address;
    quint16        m_arduino_port;

    // Application Logic
    QList<QImage> m_image_samples;
};
#endif // MAIN_WINDOW_H
