#include "HashUtils.h"

QByteArray HashUtils::calculateSHA256(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    char buffer[65536];

    while (!file.atEnd()) {
        qint64 bytes = file.read(buffer, sizeof(buffer));
        if (bytes <= 0) break;
        hash.addData(buffer, bytes);
    }

    return hash.result();
}

QString HashUtils::calculateSHA256Hex(const QString& filePath) {
    return QString(calculateSHA256(filePath).toHex());
}

bool HashUtils::verifySHA256(const QString& filePath, const QString& expectedHash) {
    QString actualHash = calculateSHA256Hex(filePath);
    return actualHash.compare(expectedHash, Qt::CaseInsensitive) == 0;
}

void HashUtils::calculateSHA256Async(const QString& filePath, 
                                      std::function<void(const QString&)> callback) {
    QThreadPool::globalInstance()->start([filePath, callback]() {
        QString hash = calculateSHA256Hex(filePath);
        QMetaObject::invokeMethod(QCoreApplication::instance(), [hash, callback]() {
            callback(hash);
        }, Qt::QueuedConnection);
    });
}