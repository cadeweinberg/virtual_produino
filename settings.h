#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFileInfo>
#include <QImageReader>
#include <QSet>
#include <QString>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT;

public:
    explicit Settings(QObject *parent = nullptr);

    static bool isImage(const QFileInfo &info) {
        return m_supported_image_extensions.contains(info.suffix());
    }

    static QString getIsImageNameFilter() { return m_is_image_name_filter; }

    static QRegularExpression getIsImageRegularExpression() { return m_is_image_regular_expression; }

private:
    static QSet<QString> buildSupportedImageExtensions() {
        QSet<QString> result;
        for (auto &&ext : QImageReader::supportedImageFormats()) {
            result.insert(QString::fromLatin1(ext).toLower());
        }
        return result;
    }

    static QString buildIsImageNameFilter() {
        QString result("Images (");
        for (auto &&ext : QImageReader::supportedImageFormats()) {
            result += "*.";
            result += QString::fromLatin1(ext).toLower();
            result += " ";
        }
        result += ")";
        return result;
    }

    static QRegularExpression buildIsImageRegex() {
        QList<QByteArray> list = QImageReader::supportedImageFormats();
        QString pattern;
        for (size_t i = 0; i < list.length(); ++i) {
            QString ext = "*." + QString::fromLatin1(list[i]).toLower();
            pattern += QRegularExpression::wildcardToRegularExpression(ext);

            if (i < (list.length() - 1)) {
                pattern += "|";
            }
        }

        return QRegularExpression{pattern};
    }

    static inline QSet<QString> m_supported_image_extensions =
        buildSupportedImageExtensions();

    static inline QString m_is_image_name_filter =
        buildIsImageNameFilter();

    static inline QRegularExpression m_is_image_regular_expression =
        buildIsImageRegex();

    QSettings m_settings;
};

#endif // SETTINGS_H
