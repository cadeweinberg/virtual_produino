#include "main_window.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QMenuBar>
#include <QDirListing>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_splitter(std::make_unique<QSplitter>())
    , m_tree_view(std::make_unique<QTreeView>())
    , m_table_view(std::make_unique<QTableView>())
    , m_preview(std::make_unique<QLabel>())
    , m_item_selection_model(std::make_unique<QItemSelectionModel>())
    , m_filesystem_model(std::make_unique<QFileSystemModel>())
    , m_root_path(QDir::homePath())
    , m_df_socket()
    , m_df_address(QHostAddress::LocalHost)
    , m_df_port(8888)
    , m_df_frame(0)
    , m_arduino_socket()
    , m_arduino_address(QHostAddress("192.168.0.101"))
    , m_arduino_port(50505)
    , m_image_samples()
{
    // UI
    setupUI();

    // Filesystem
    setupFilesystem();

    // Networking
    setupNetworking();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Main Menu
    QMenuBar      *menu = menuBar();
    QMenu         *file = menu->addMenu("File");
    QAction *actionNew  = file->addAction("New");
    QAction *actionOpen = file->addAction("Open");
    QAction *actionDir  = file->addAction("Open Directory");

    connect(actionNew, &QAction::triggered, this, &MainWindow::onNew);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(actionDir, &QAction::triggered, this, &MainWindow::onOpenDirectory);

    // Central Widget
    setCentralWidget(m_splitter.get());
    m_splitter->addWidget(m_tree_view.get());
    m_splitter->addWidget(m_preview.get());

    //m_splitter->addWidget(m_table_view.get());
}

void MainWindow::setupFilesystem()
{
    m_tree_view->setModel(m_filesystem_model.get());
    m_tree_view->setSelectionModel(m_item_selection_model.get());
    // QAbstractItemView::clicked/doubleClicked
    connect(m_tree_view.get(), &QTreeView::clicked, this, &MainWindow::onFileViewItemClicked);

    updateDirectoryListing();
}

void MainWindow::setupNetworking()
{
    m_df_socket.bind(m_df_address, m_df_port);
    connect(&m_df_socket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
}

void MainWindow::onNew()
{

}

void MainWindow::onOpen()
{

}

void MainWindow::onOpenDirectory()
{
    m_root_path = QFileDialog::getExistingDirectory(this);
    updateDirectoryListing();
    populateImageSamples();
}

void MainWindow::onFileViewItemClicked(const QModelIndex &index)
{
    QString path = m_filesystem_model->filePath(index);
    previewImage(path);
}

void MainWindow::onSelectionChanged(const QModelIndex &selected, const QModelIndex &deselected)
{
   QString path = m_filesystem_model->filePath(selected);
   previewImage(path);
}

void MainWindow::updateDirectoryListing()
{
    m_filesystem_model->setRootPath(m_root_path);
    m_tree_view->setRootIndex(m_filesystem_model->index(m_root_path));
}

void MainWindow::previewImage(const QString &path)
{
    QImage image(path);
    if (!image.isNull()) {
        QImage scaled = image.scaledToWidth(512);
        m_preview->setPixmap(QPixmap::fromImage(scaled));
    }
}

void MainWindow::previewImage()
{
    QImage const &image = currentImageSample();
    if (!image.isNull()) {
        m_preview->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::readPendingDatagrams()
{
    while (m_df_socket.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_df_socket.pendingDatagramSize());
        QHostAddress sender_address;
        quint16 sender_port;

        m_df_socket.readDatagram(datagram.data(), datagram.size(), &sender_address, &sender_port);

        if (sender_address != m_df_address || sender_port != m_df_port)
        {
            m_df_address = sender_address;
            m_df_port = sender_port;
        }

        QJsonDocument doc = QJsonDocument::fromJson(datagram);
        QJsonObject   obj = doc.object();
        if (!obj.isEmpty() && obj["event"].isString())
        {
            receiveEvent(obj);
        }
    }
}

void MainWindow::populateImageSamples()
{
    m_image_samples.clear();
    QDirListing::IteratorFlags flags = QDirListing::IteratorFlag::FilesOnly;
    QDirListing dir(m_root_path);

    for (QDirListing::DirEntry const &entry : dir) {
        QImage image(entry.absoluteFilePath());
        if (image.isNull()) { continue; }
        m_image_samples.push_back(sampleImage(image));
    }
}

QImage MainWindow::sampleImage(QImage const &image)
{
    return image.scaled(QSize(10, 10), Qt::IgnoreAspectRatio).convertToFormat(QImage::Format_RGB888);
}

bool MainWindow::writeFrame()
{
    m_arduino_socket.connectToHost(m_arduino_address, m_arduino_port);

    QImage sample = currentImageSample();
    char * bits = reinterpret_cast<char *>(sample.bits());
    qsizetype length = sample.sizeInBytes();
    qsizetype written = 0;

    do {
      written += m_arduino_socket.write(bits + written, length - written);
    } while (written != length);

    m_arduino_socket.disconnectFromHost();
    return true;
}

QImage const &MainWindow::currentImageSample()
{
    return m_image_samples.at(m_df_frame % m_image_samples.count());
}


void MainWindow::sendJson(const QJsonObject& obj)
{
    if (m_df_port)
    {
        QJsonDocument doc(obj);
        m_df_socket.writeDatagram(doc.toJson(), m_df_address, m_df_port);
    }
}

void MainWindow::receiveEvent(const QJsonObject& obj)
{
    QString event = obj["event"].toString();
    if (event == "hello")
    {
        // do handshake
        QJsonObject response;
        response["command"] = QString("hello");
        response["version"] = 1.0;
        sendJson(response);
        return;
    }

    if (event == "position") {
        m_df_frame = obj["frame"].toInteger();
        previewImage();
        writeFrame();
        return;
    }

    if (event == "captureComplete") {
        m_df_frame = obj["frame"].toInteger();
        previewImage();
        writeFrame();
        return;
    }

    if (event == "viewFrame")
    {
        m_df_frame = obj["frame"].toInteger();
        previewImage();
        writeFrame();
        return;
    }
}

