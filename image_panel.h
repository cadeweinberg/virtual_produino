#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

#include <QFileInfo>
#include <QLabel>
#include <QImage>

/**
 * @brief The ImagePanel class provides the program with a downsampled
 * version of the selected image for display. and a sampling of the image
 * for sending to the Arduino.
 */
class ImagePanel : public QLabel
{
    Q_OBJECT
public:
    ImagePanel(int width, int height, QWidget *parent = nullptr);

    void image(const QString &path);
    void image(const QFileInfo &info);
    void image(QImage image);
    const QImage &image() const;

    QImage downsample(int width) const;
    QImage downsample(int width, int height) const;

private:
    int    m_width;
    int    m_height;
    QImage m_image;
};

#endif // IMAGE_PANEL_H
