#include "PathUtils.h"
#include <QStandardPaths>
#include <QRegularExpression>

QString PathUtils::getAppDataPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString PathUtils::getTempPath() {
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}

QString PathUtils::getDownloadsPath() {
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

QString PathUtils::getDesktopPath() {
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString PathUtils::getDocumentsPath() {
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

QString PathUtils::ensureDirectoryExists(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path;
}

bool PathUtils::removeDirectory(const QString& path) {
    QDir dir(path);
    return dir.removeRecursively();
}

QString PathUtils::sanitizeFileName(const QString& fileName) {
    QString result = fileName;
    result.remove(QRegularExpression(R"([\\/:*?"<>|])"));
    return result;
}

bool PathUtils::isPathValid(const QString& path) {
    QFileInfo info(path);
    return info.exists();
}