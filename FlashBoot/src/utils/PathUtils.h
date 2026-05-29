#ifndef PATHUTILS_H
#define PATHUTILS_H

#include "pch.h"

class PathUtils {
public:
    static QString getAppDataPath();
    static QString getTempPath();
    static QString getDownloadsPath();
    static QString getDesktopPath();
    static QString getDocumentsPath();
    static QString ensureDirectoryExists(const QString& path);
    static bool removeDirectory(const QString& path);
    static QString sanitizeFileName(const QString& fileName);
    static bool isPathValid(const QString& path);
};

#endif // PATHUTILS_H