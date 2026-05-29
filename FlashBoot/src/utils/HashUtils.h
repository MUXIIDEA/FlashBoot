#ifndef HASHUTILS_H
#define HASHUTILS_H

#include "pch.h"

class HashUtils {
public:
    static QByteArray calculateSHA256(const QString& filePath);
    static QString calculateSHA256Hex(const QString& filePath);
    static bool verifySHA256(const QString& filePath, const QString& expectedHash);
    static void calculateSHA256Async(const QString& filePath, 
                                     std::function<void(const QString&)> callback);
};

#endif // HASHUTILS_H