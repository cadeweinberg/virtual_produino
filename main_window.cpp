#include "main_window.h"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_splitter(new QSplitter(this))
    , m_filesystem_widget(new FilesystemWidget(this))
{
    setupUI();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUI()
{
    QMenuBar      *menu    = menuBar();
    QMenu         *file    = menu->addMenu("File");
    QAction *actionOpenDirectory = file->addAction("Open Directory");
    connect(actionOpenDirectory, &QAction::triggered, this, &MainWindow::onOpenDirectory);

    m_splitter->addWidget(m_filesystem_widget);
    // #NOTE: global configuration/preferences object? for setting/getting ip-addresses etc?

    setCentralWidget(m_splitter);
}

void MainWindow::onOpenDirectory()
{
    QString caption     = "Open a Directory";
    QString root_path   = m_filesystem_widget->getRootPath();
    const QString &path = QFileDialog::getExistingDirectory(this, caption, root_path);
    m_filesystem_widget->setRootPath(path);
}

//
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QDirListing>
// #include <QStringList>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
//     , m_root_path(QDir::homePath())
//     , m_image_bindings()
//     , m_splitter(std::make_unique<QSplitter>())
//     , m_filesystem_view(std::make_unique<QTreeView>())
//     , m_image_bindings_view(std::make_unique<QListView>())
//     , m_image_preview(std::make_unique<QLabel>())
//     , m_filesystem_model(std::make_unique<QFileSystemModel>())
//     , m_image_bindings_adapter(std::make_unique<ImageBindingsAdapter>(m_image_bindings))
//     , m_df_socket()
//     , m_df_address(QHostAddress::LocalHost)
//     , m_df_port(8888)
//     , m_df_frame(0)
//     , m_arduino_socket()
//     , m_arduino_address(QHostAddress("192.168.0.101"))
//     , m_arduino_port(50505)
// {
//     // UI
//     setupUI();

//     // Filesystem
//     setupFilesystem();

//     // Networking
//     setupNetworking();
// }

// MainWindow::~MainWindow()
// {
// }

// void MainWindow::setupUI()
// {
//     // Main Menu
//     QMenuBar      *menu = menuBar();
//     QMenu         *file = menu->addMenu("File");
//     QAction *actionNew  = file->addAction("New");
//     QAction *actionOpen = file->addAction("Open");
//     QAction *actionDir  = file->addAction("Open Directory");

//     connect(actionNew, &QAction::triggered, this, &MainWindow::onNew);
//     connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
//     connect(actionDir, &QAction::triggered, this, &MainWindow::onOpenDirectory);

//     // Central Widget
//     setCentralWidget(m_splitter.get());
//     m_splitter->addWidget(m_filesystem_view.get());
//     m_splitter->addWidget(m_image_bindings_view.get());
//     m_splitter->addWidget(m_image_preview.get());

//     m_image_bindings_view->setModel(m_image_bindings_adapter->model());
//     m_filesystem_view->setModel(m_filesystem_model.get());
// }

// void MainWindow::setupFilesystem()
// {

//     //m_tree_view->setSelectionModel(m_item_selection_model.get());
//     // QAbstractItemView::clicked/doubleClicked
//     //connect(m_tree_view.get(), &QTreeView::clicked, this, &MainWindow::onFileViewItemClicked);
//     //connect(m_tree_view.get(), &QTreeView::selectionChanged, );

//     updateDirectoryListing();
// }

// void MainWindow::setupNetworking()
// {
//     m_df_socket.bind(m_df_address, m_df_port);
//     connect(&m_df_socket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
// }

// void MainWindow::onNew()
// {

// }

// void MainWindow::onOpen()
// {

// }

// void MainWindow::onOpenDirectory()
// {
//     m_root_path = QFileDialog::getExistingDirectory(this);
//     updateDirectoryListing();
//     populateImageSamples();
// }

// void MainWindow::onFileViewItemClicked(const QModelIndex &index)
// {
//     QString path = m_filesystem_model->filePath(index);
//     previewImage(path);
// }

// void MainWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
// {
//    //QString path = m_filesystem_model->filePath(selected.first().topLeft());
//    //previewImage(path);
// }

// void MainWindow::updateDirectoryListing()
// {
//     m_filesystem_model->setRootPath(m_root_path);
//     m_filesystem_view->setRootIndex(m_filesystem_model->index(m_root_path));
// }

// void MainWindow::previewImage(const QString &path)
// {
//     QImage image(path);
//     if (!image.isNull()) {
//         QImage scaled = image.scaledToWidth(512);
//         m_image_preview->setPixmap(QPixmap::fromImage(scaled));
//     }
// }

// void MainWindow::previewImage()
// {
//     QImage const &image = currentImageSample();
//     if (!image.isNull()) {
//         m_image_preview->setPixmap(QPixmap::fromImage(image));
//     }
// }

// void MainWindow::readPendingDatagrams()
// {
//     while (m_df_socket.hasPendingDatagrams())
//     {
//         QByteArray datagram;
//         datagram.resize(m_df_socket.pendingDatagramSize());
//         QHostAddress sender_address;
//         quint16 sender_port;

//         m_df_socket.readDatagram(datagram.data(), datagram.size(), &sender_address, &sender_port);

//         if (sender_address != m_df_address || sender_port != m_df_port)
//         {
//             m_df_address = sender_address;
//             m_df_port = sender_port;
//         }

//         QJsonDocument doc = QJsonDocument::fromJson(datagram);
//         QJsonObject   obj = doc.object();
//         if (!obj.isEmpty() && obj["event"].isString())
//         {
//             receiveEvent(obj);
//         }
//     }
// }

// void MainWindow::populateImageSamples()
// {
//     m_image_bindings.clear();
//     QDirListing::IteratorFlags flags = QDirListing::IteratorFlag::FilesOnly;
//     QDirListing dir(m_root_path);

//     quint64 index = 0;
//     for (QDirListing::DirEntry const &entry : dir) {
//         QImage image(entry.absoluteFilePath());
//         if (image.isNull()) { continue; }
//         m_image_bindings.push_back(std::make_pair(index++, sampleImage(image)));
//     }
// }

// QImage MainWindow::sampleImage(QImage const &image)
// {
//     return image.scaled(QSize(10, 10), Qt::IgnoreAspectRatio).convertToFormat(QImage::Format_RGB888);
// }

// bool MainWindow::writeFrame()
// {
//     m_arduino_socket.connectToHost(m_arduino_address, m_arduino_port);

//     QImage sample = currentImageSample();
//     char * bits = reinterpret_cast<char *>(sample.bits());
//     qsizetype length = sample.sizeInBytes();
//     qsizetype written = 0;

//     do {
//       written += m_arduino_socket.write(bits + written, length - written);
//     } while (written != length);

//     m_arduino_socket.disconnectFromHost();
//     return true;
// }

// QImage const &MainWindow::currentImageSample()
// {
//     ensureEnoughImages();
//     auto const &pair = m_image_bindings.at(m_df_frame);
//     Q_ASSERT_X(pair.first == m_df_frame,
//                "MainWindow::currentImageSample",
//                "Sampled image's frame does not match current frame");
//     return pair.second;
// }

// QImage MainWindow::blankImage()
// {
//     QImage result(10, 10, QImage::Format_RGB888);
//     result.fill(QColor{0, 0, 0});
//     return result;
// }

// void MainWindow::ensureEnoughImages()
// {
//     if (!needMoreImages()) { return; }

//     for (quint64 i = m_image_bindings.count(); i < m_df_frame + 1; ++i) {
//         m_image_bindings.push_back(std::make_pair(i, blankImage()));
//     }
// }

// bool MainWindow::needMoreImages()
// {
//     return m_df_frame >= m_image_bindings.count();
// }

// void MainWindow::sendJson(const QJsonObject& obj)
// {
//     if (m_df_port)
//     {
//         QJsonDocument doc(obj);
//         m_df_socket.writeDatagram(doc.toJson(), m_df_address, m_df_port);
//     }
// }

// void MainWindow::receiveEvent(const QJsonObject& obj)
// {
//     QString event = obj["event"].toString();
//     if (event == "hello")
//     {
//         // do handshake
//         QJsonObject response;
//         response["command"] = QString("hello");
//         response["version"] = 1.0;
//         sendJson(response);
//         return;
//     }

//     if (event == "position") {
//         m_df_frame = obj["frame"].toInteger();
//         previewImage();
//         writeFrame();
//         return;
//     }

//     if (event == "captureComplete") {
//         m_df_frame = obj["frame"].toInteger();
//         previewImage();
//         writeFrame();
//         return;
//     }

//     if (event == "viewFrame")
//     {
//         m_df_frame = obj["frame"].toInteger();
//         previewImage();
//         writeFrame();
//         return;
//     }
// }

