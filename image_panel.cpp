#include "image_panel.h"

#include <QFileInfo>

#include "settings.h"

ImagePanel::ImagePanel(int width, int height, QWidget *parent)
    : QLabel(parent)
    , m_width(width)
    , m_height(height)
    , m_image()
{
    resize(m_width, m_height);
}

void ImagePanel::image(const QString &path)
{
    image(QFileInfo(path));
}

void ImagePanel::image(const QFileInfo &info) {
    if (!info.exists()) { return; }
    if (!info.isFile()) { return; }
    if (!Settings::isImage(info)) { return; }

    image(QImage(info.absoluteFilePath()));
}

void ImagePanel::image(QImage image)
{
    m_image = std::move(image);
    setPixmap(QPixmap::fromImage(m_image.scaled(m_width, m_height)));
}

const QImage &ImagePanel::image() const
{
    return m_image;
}

QImage ImagePanel::downsample(int width) const
{
    return m_image.scaledToWidth(width);
}

QImage ImagePanel::downsample(int width, int height) const
{
    return m_image.scaled(width, height);
}
