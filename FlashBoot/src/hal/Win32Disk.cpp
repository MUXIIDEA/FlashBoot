#include "Win32Disk.h"
#include <QProcess>

bool Win32Disk::getDiskInfo(int diskNumber, QString& model, QString& serial,
                            quint64& size, bool& isRemovable) {
    QString path = QString("\\\\.\\PhysicalDrive%1").arg(diskNumber);
    HANDLE hDisk = CreateFileW(path.toStdWString().c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr,
                               OPEN_EXISTING,
                               0,
                               nullptr);

    if (hDisk == INVALID_HANDLE_VALUE) {
        return false;
    }

    STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageAccessAlignmentProperty;
    query.QueryType = PropertyStandardQuery;

    STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR alignmentDesc;
    DWORD bytesReturned;

    if (!DeviceIoControl(hDisk, IOCTL_STORAGE_QUERY_PROPERTY,
                         &query, sizeof(query),
                         &alignmentDesc, sizeof(alignmentDesc),
                         &bytesReturned, nullptr)) {
        CloseHandle(hDisk);
        return false;
    }

    STORAGE_DEVICE_DESCRIPTOR deviceDesc;
    ZeroMemory(&deviceDesc, sizeof(deviceDesc));
    deviceDesc.Size = sizeof(deviceDesc);

    query.PropertyId = StorageDeviceProperty;

    if (!DeviceIoControl(hDisk, IOCTL_STORAGE_QUERY_PROPERTY,
                         &query, sizeof(query),
                         &deviceDesc, sizeof(deviceDesc),
                         &bytesReturned, nullptr)) {
        CloseHandle(hDisk);
        return false;
    }

    if (deviceDesc.ProductIdOffset != 0) {
        model = QString::fromWCharArray(reinterpret_cast<WCHAR*>(reinterpret_cast<BYTE*>(&deviceDesc) + deviceDesc.ProductIdOffset));
    }
    if (deviceDesc.SerialNumberOffset != 0) {
        serial = QString::fromWCharArray(reinterpret_cast<WCHAR*>(reinterpret_cast<BYTE*>(&deviceDesc) + deviceDesc.SerialNumberOffset));
    }

    DISK_GEOMETRY_EX geometry;
    if (DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                        nullptr, 0,
                        &geometry, sizeof(geometry),
                        &bytesReturned, nullptr)) {
        size = geometry.DiskSize.QuadPart;
    }

    isRemovable = (deviceDesc.RemovableMedia != FALSE);

    CloseHandle(hDisk);
    return true;
}

bool Win32Disk::getPartitionInfo(int diskNumber, QString& partitionType) {
    QString path = QString("\\\\.\\PhysicalDrive%1").arg(diskNumber);
    HANDLE hDisk = CreateFileW(path.toStdWString().c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr,
                               OPEN_EXISTING,
                               0,
                               nullptr);

    if (hDisk == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesReturned;
    DISK_PARTITION_INFO partitionInfo;
    ZeroMemory(&partitionInfo, sizeof(partitionInfo));

    if (!DeviceIoControl(hDisk, IOCTL_DISK_GET_PARTITION_INFO_EX,
                         nullptr, 0,
                         &partitionInfo, sizeof(partitionInfo),
                         &bytesReturned, nullptr)) {
        CloseHandle(hDisk);
        return false;
    }

    if (partitionInfo.PartitionStyle == PARTITION_STYLE_MBR) {
        partitionType = "MBR";
    } else if (partitionInfo.PartitionStyle == PARTITION_STYLE_GPT) {
        partitionType = "GPT";
    } else {
        partitionType = "Unknown";
    }

    CloseHandle(hDisk);
    return true;
}

bool Win32Disk::formatDisk(int diskNumber, const QString& fileSystem,
                           const QString& volumeLabel, bool quickFormat) {
    Q_UNUSED(diskNumber);
    Q_UNUSED(fileSystem);
    Q_UNUSED(volumeLabel);
    Q_UNUSED(quickFormat);
    return true;
}

bool Win32Disk::writeMBR(int diskNumber, const QString& mbrType) {
    Q_UNUSED(diskNumber);
    Q_UNUSED(mbrType);
    return true;
}

bool Win32Disk::writeBootSector(int diskNumber, int partitionIndex) {
    Q_UNUSED(diskNumber);
    Q_UNUSED(partitionIndex);
    return true;
}

bool Win32Disk::lockVolume(const QString& driveLetter) {
    QString path = QString("\\\\.\\%1:").arg(driveLetter);
    HANDLE hVolume = CreateFileW(path.toStdWString().c_str(),
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_EXISTING,
                                 0,
                                 nullptr);

    if (hVolume == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesReturned;
    bool success = DeviceIoControl(hVolume, FSCTL_LOCK_VOLUME,
                                   nullptr, 0,
                                   nullptr, 0,
                                   &bytesReturned, nullptr);

    CloseHandle(hVolume);
    return success;
}

bool Win32Disk::unlockVolume(const QString& driveLetter) {
    QString path = QString("\\\\.\\%1:").arg(driveLetter);
    HANDLE hVolume = CreateFileW(path.toStdWString().c_str(),
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_EXISTING,
                                 0,
                                 nullptr);

    if (hVolume == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesReturned;
    bool success = DeviceIoControl(hVolume, FSCTL_UNLOCK_VOLUME,
                                   nullptr, 0,
                                   nullptr, 0,
                                   &bytesReturned, nullptr);

    CloseHandle(hVolume);
    return success;
}

bool Win32Disk::dismountVolume(const QString& driveLetter) {
    QString path = QString("\\\\.\\%1:").arg(driveLetter);
    HANDLE hVolume = CreateFileW(path.toStdWString().c_str(),
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_EXISTING,
                                 0,
                                 nullptr);

    if (hVolume == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesReturned;
    bool success = DeviceIoControl(hVolume, FSCTL_DISMOUNT_VOLUME,
                                   nullptr, 0,
                                   nullptr, 0,
                                   &bytesReturned, nullptr);

    CloseHandle(hVolume);
    return success;
}

bool Win32Disk::formatVolume(const QString& driveLetter, const QString& fileSystem,
                             const QString& volumeLabel, bool quickFormat) {
    Q_UNUSED(driveLetter);
    Q_UNUSED(fileSystem);
    Q_UNUSED(volumeLabel);
    Q_UNUSED(quickFormat);
    return true;
}