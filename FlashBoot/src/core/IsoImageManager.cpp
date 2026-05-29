#include "IsoImageManager.h"

IsoImageManager::IsoImageManager(QObject* parent) : QObject(parent) {
}

IsoInfo IsoImageManager::parseIso(const QString& filePath) {
    IsoInfo info;
    info.filePath = filePath;
    info.fileName = QFileInfo(filePath).fileName();
    info.fileSize = QFileInfo(filePath).size();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        info.errorString = QObject::tr("Cannot open file: %1").arg(file.errorString());
        return info;
    }

    file.seek(16 * 2048);
    QByteArray sector = file.read(2048);

    if (sector.size() < 2048) {
        info.errorString = QObject::tr("File too small to be a valid ISO");
        return info;
    }

    if (sector[0] != 0x01 || sector[1] != 'C' || sector[2] != 'D' ||
        sector[3] != '0' || sector[4] != '0' || sector[5] != '1') {
        info.errorString = QObject::tr("Invalid ISO 9660 signature");
        return info;
    }

    QByteArray volumeLabel = sector.mid(40, 32);
    int endPos = volumeLabel.indexOf('\x00');
    if (endPos < 0) endPos = volumeLabel.size();
    while (endPos > 0 && volumeLabel[endPos - 1] == ' ') endPos--;
    info.displayName = QString::fromLatin1(volumeLabel.left(endPos));

    info.bootMode = detectBootMode(filePath);
    info.arch = detectArch(filePath);
    info.version = extractVersionFromIso(filePath);

    info.isValid = true;
    return info;
}

void IsoImageManager::verifyIsoAsync(const QString& filePath) {
    QThreadPool::globalInstance()->start([this, filePath]() {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            emit errorOccurred(QObject::tr("Cannot open file"));
            return;
        }

        QCryptographicHash hash(QCryptographicHash::Sha256);
        quint64 totalSize = file.size();
        quint64 bytesRead = 0;
        char buffer[65536];

        while (!file.atEnd()) {
            qint64 bytes = file.read(buffer, sizeof(buffer));
            if (bytes <= 0) break;
            hash.addData(buffer, bytes);
            bytesRead += bytes;
            emit verificationProgress(static_cast<int>((bytesRead * 100) / totalSize));
        }

        QString hashStr = QString(hash.result().toHex());
        emit verificationFinished(true, hashStr);
    });
}

bool IsoImageManager::isBootable(const QString& filePath) {
    return detectBootMode(filePath) != BootMode::Unknown;
}

BootMode IsoImageManager::detectBootMode(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return BootMode::Unknown;

    file.seek(17 * 2048);
    QByteArray bootRecord = file.read(2048);

    if (bootRecord.size() >= 8 &&
        bootRecord.mid(0, 8) == "EL TORITO SPECIFICATION") {
        file.seek(0);
        QByteArray header = file.read(65536);

        bool hasUefi = header.contains("efi/boot/bootx64.efi") ||
                        header.contains("EFI/BOOT/BOOTX64.EFI") ||
                        header.contains("efi/boot/bootia32.efi");

        bool hasBios = header.contains("isolinux.bin") ||
                       header.contains("bootmgr") ||
                       header.contains("ntldr");

        if (hasUefi && hasBios) return BootMode::Dual;
        if (hasUefi) return BootMode::UEFI;
        if (hasBios) return BootMode::BIOS;
    }

    return BootMode::Unknown;
}

IsoInfo IsoImageManager::parseIso9660(const QString& path) {
    return IsoInfo::fromFile(path);
}

QString IsoImageManager::extractVersionFromIso(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return "";

    file.seek(16 * 2048);
    QByteArray sector = file.read(2048);

    QByteArray versionField = sector.mid(40, 32);
    QString versionStr = QString::fromLatin1(versionField).trimmed();

    QRegularExpression rx(R"(Windows (\d{1,2}))");
    QRegularExpressionMatch match = rx.match(versionStr);
    if (match.hasMatch()) {
        return QString("Windows %1").arg(match.captured(1));
    }

    return versionStr;
}

ArchType IsoImageManager::detectArch(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return ArchType::Unknown;

    QByteArray header = file.read(262144);

    if (header.contains("amd64") || header.contains("x64") ||
        header.contains("BOOTX64.EFI")) {
        return ArchType::x64;
    } else if (header.contains("x86") || header.contains("i386") ||
               header.contains("BOOTIA32.EFI")) {
        return ArchType::x86;
    } else if (header.contains("arm64") || header.contains("aarch64") ||
               header.contains("BOOTAA64.EFI")) {
        return ArchType::ARM64;
    }

    return ArchType::Unknown;
}