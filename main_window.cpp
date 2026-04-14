#include "main_window.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_splitter(std::make_unique<QSplitter>())
    , m_tree_view(std::make_unique<QTreeView>())
    , m_table_view(std::make_unique<QTableView>())
    , m_preview(std::make_unique<QLabel>())
    , m_item_selection_model(std::make_unique<QItemSelectionModel>())
    , m_filesystem_model(std::make_unique<QFileSystemModel>())
    , m_root_path(QDir::homePath())
    , m_socket()
    , m_df_address(QHostAddress::LocalHost)
    , m_df_port(8888)
    , m_df_frame(0)
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
    m_socket.bind(m_df_address, m_df_port);
    connect(&m_socket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
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
    QFile file(path);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODeviceBase::ReadOnly)) {
        return;
    }

    QPicture picture;
    if (picture.load(&file)) {
        m_preview->setPicture(picture);
    }
}

void MainWindow::readPendingDatagrams()
{
    while (m_socket.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_socket.pendingDatagramSize());
        QHostAddress sender_address;
        quint16 sender_port;

        m_socket.readDatagram(datagram.data(), datagram.size(), &sender_address, &sender_port);

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


void MainWindow::sendJson(const QJsonObject& obj)
{
    if (m_df_port)
    {
        QJsonDocument doc(obj);
        m_socket.writeDatagram(doc.toJson(), m_df_address, m_df_port);
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

    if (event == "viewFrame")
    {
        m_df_frame = obj["frame"].toInteger();
        return;
    }
}

