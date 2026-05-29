#ifndef WIN32DISK_H
#define WIN32DISK_H

#include "pch.h"

class Win32Disk {
public:
    static bool getDiskInfo(int diskNumber, QString& model, QString& serial, 
                           quint64& size, bool& isRemovable);
    static bool getPartitionInfo(int diskNumber, QString& partitionType);
    static bool formatDisk(int diskNumber, const QString& fileSystem, 
                          const QString& volumeLabel, bool quickFormat);
    static bool writeMBR(int diskNumber, const QString& mbrType);
    static bool writeBootSector(int diskNumber, int partitionIndex);
    static bool lockVolume(const QString& driveLetter);
    static bool unlockVolume(const QString& driveLetter);
    static bool dismountVolume(const QString& driveLetter);
    static bool formatVolume(const QString& driveLetter, const QString& fileSystem,
                            const QString& volumeLabel, bool quickFormat);
};

#endif // WIN32DISK_H