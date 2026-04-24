#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include "file_system_panel.h"
#include "image_selection_panel.h"
#include "dragonframe_socket.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void setupUI();
    void setupNetworking();

    void onOpen();
    void onOpenDirectory();
    void onSave();
    void onDoubleClickFileSystemEntry(const QModelIndex &index);
    void onAddSelected();
    void onDragonframeRead();

    void dragonframeEvent(const QJsonObject &json);
    void dragonframeResponse(const QJsonObject &json);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    QSplitter           *m_splitter;
    FileSystemPanel     *m_filesystem_panel;
    ImageSelectionPanel *m_image_selection_panel;
    DragonframeSocket   *m_dragonframe_socket;
};
#endif // MAIN_WINDOW_H


// #include <QUdpSocket>
// #include <QTcpSocket>
// #include <QRangeModel>
// #include <QRangeModelAdapter>

// #include <QListView>
// #include <QLabel>
// #include <QImage>
// #include <QList>

    /*// UI
    void onNew();
    void onOpen();
    void onOpenDirectory();
    void onFileViewItemClicked(const QModelIndex &index);
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

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

    QImage blankImage();
    bool needMoreImages();
    void ensureEnoughImages();


    using ImageBinding = std::pair<Frame, QImage>;
    using ImageBindingList = QList<ImageBinding>;
    using ImageBindingsAdapter = QRangeModelAdapter<ImageBindingList>;

    QString          m_root_path;
    ImageBindingList m_image_bindings;

    std::unique_ptr<QFileSystemModel>     m_filesystem_model;
    std::unique_ptr<ImageBindingsAdapter> m_image_bindings_adapter;

    QUdpSocket     m_df_socket;
    QHostAddress   m_df_address;
    quint16        m_df_port;
    Frame          m_df_frame;
    QTcpSocket     m_arduino_socket;
    QHostAddress   m_arduino_address;
    quint16        m_arduino_port;

    std::unique_ptr<QSplitter>  m_splitter;
*/

