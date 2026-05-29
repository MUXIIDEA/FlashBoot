#ifndef ISOINFO_H
#define ISOINFO_H

#include "pch.h"

enum class BootMode {
    BIOS,
    UEFI,
    Dual,
    Unknown
};

enum class ArchType {
    x86,
    x64,
    ARM64,
    Unknown
};

class IsoInfo {
public:
    QString filePath;
    QString fileName;
    QString displayName;
    QString version;
    QString buildNumber;
    ArchType arch = ArchType::Unknown;
    BootMode bootMode = BootMode::Unknown;
    quint64 fileSize = 0;
    QByteArray sha256;
    bool isValid = false;
    QString errorString;

    static IsoInfo fromFile(const QString& path);

    QString archString() const {
        switch (arch) {
            case ArchType::x86: return "x86";
            case ArchType::x64: return "x64";
            case ArchType::ARM64: return "ARM64";
            default: return "Unknown";
        }
    }

    QString bootModeString() const {
        switch (bootMode) {
            case BootMode::BIOS: return "Legacy BIOS";
            case BootMode::UEFI: return "UEFI Only";
            case BootMode::Dual: return "BIOS + UEFI";
            default: return "Unknown";
        }
    }

    QString fileSizeString() const {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = static_cast<double>(fileSize);
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        return QString("%1 %2").arg(size, 0, 'f', 2).arg(units[unitIndex]);
    }

    QString sha256String() const {
        return QString(sha256.toHex());
    }
};

Q_DECLARE_METATYPE(IsoInfo)

#endif // ISOINFO_H