#ifndef IMAGE_FORMAT_H
#define IMAGE_FORMAT_H

#include <QFileInfo>
#include <QImageReader>
#include <QSet>
#include <QString>

class ImageFormat
{
public:
    static bool isSupported(const QFileInfo &info) {
        return m_supported_image_extensions.contains(info.suffix());
    }

private:
    static QSet<QString> getSupportedImageExtensions() {
        QSet<QString> result;
        for (auto &&ext : QImageReader::supportedImageFormats()) {
            result.insert(QString::fromLatin1(ext).toLower());
        }
        return result;
    }

    static inline QSet<QString> m_supported_image_extensions =
        getSupportedImageExtensions();
};

#endif // IMAGE_FORMAT_H
