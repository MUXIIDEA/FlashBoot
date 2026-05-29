#ifndef USBDEVICE_H
#define USBDEVICE_H

#include "pch.h"

enum class PartitionScheme {
    MBR,
    GPT,
    Unknown
};

enum class FileSystem {
    FAT32,
    NTFS,
    exFAT,
    UDF,
    Unknown
};

enum class DeviceStatus {
    Ready,
    Busy,
    Error,
    WriteProtected,
    Unknown
};

class UsbDevice {
public:
    QString deviceId;
    QString physicalPath;
    QString driveLetter;
    QString vendor;
    QString product;
    QString serialNumber;
    quint64 totalBytes = 0;
    quint64 freeBytes = 0;
    FileSystem fileSystem = FileSystem::Unknown;
    PartitionScheme partitionScheme = PartitionScheme::Unknown;
    DeviceStatus status = DeviceStatus::Unknown;
    bool isRemovable = true;
    bool isUsb = true;
    int diskNumber = -1;

    int usedPercent() const {
        if (totalBytes == 0) return 0;
        return static_cast<int>((totalBytes - freeBytes) * 100 / totalBytes);
    }

    QString capacityString() const {
        return QString("%1 / %2")
            .arg(formatBytes(totalBytes - freeBytes))
            .arg(formatBytes(totalBytes));
    }

    static QString formatBytes(quint64 bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = static_cast<double>(bytes);
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        return QString("%1 %2").arg(size, 0, 'f', 2).arg(units[unitIndex]);
    }

    QString fileSystemString() const {
        switch (fileSystem) {
            case FileSystem::FAT32: return "FAT32";
            case FileSystem::NTFS: return "NTFS";
            case FileSystem::exFAT: return "exFAT";
            case FileSystem::UDF: return "UDF";
            default: return "Unknown";
        }
    }

    QString partitionSchemeString() const {
        switch (partitionScheme) {
            case PartitionScheme::MBR: return "MBR";
            case PartitionScheme::GPT: return "GPT";
            default: return "Unknown";
        }
    }

    QString statusString() const {
        switch (status) {
            case DeviceStatus::Ready: return "Ready";
            case DeviceStatus::Busy: return "Busy";
            case DeviceStatus::Error: return "Error";
            case DeviceStatus::WriteProtected: return "Write Protected";
            default: return "Unknown";
        }
    }
};

Q_DECLARE_METATYPE(UsbDevice)

#endif // USBDEVICE_H