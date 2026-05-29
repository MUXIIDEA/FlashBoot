#include "DeviceEnumerator.h"

QList<UsbDevice> DeviceEnumerator::enumerateUsbDevices() {
    QList<UsbDevice> devices;

#ifdef Q_OS_WIN
    GUID guid = GUID_DEVINTERFACE_DISK;
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&guid, nullptr, nullptr,
                                              DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return devices;
    }

    SP_DEVICE_INTERFACE_DATA interfaceData;
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD index = 0;
    while (SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &guid, index++, &interfaceData)) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, nullptr, 0, &requiredSize, nullptr);

        if (requiredSize == 0) continue;

        std::vector<BYTE> detailDataBuffer(requiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W detailData =
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(detailDataBuffer.data());
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA devInfoData;
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, detailData,
                                              requiredSize, nullptr, &devInfoData)) {
            UsbDevice dev;
            dev.deviceId = getDeviceInstanceId(hDevInfo, devInfoData);
            dev.product = getDeviceProperty(hDevInfo, devInfoData, SPDRP_FRIENDLYNAME);
            dev.vendor = getDeviceProperty(hDevInfo, devInfoData, SPDRP_MFG);

            int driveLetter = getDriveLetterFromDeviceId(dev.deviceId);
            if (driveLetter >= 0) {
                dev.driveLetter = QString("%1:").arg(QChar('A' + driveLetter));

                ULARGE_INTEGER freeBytes, totalBytes;
                QString path = QString("%1:\\").arg(QChar('A' + driveLetter));
                if (GetDiskFreeSpaceExW(path.toStdWString().c_str(),
                                        &freeBytes, &totalBytes, nullptr)) {
                    dev.totalBytes = totalBytes.QuadPart;
                    dev.freeBytes = freeBytes.QuadPart;
                }

                WCHAR fsName[MAX_PATH + 1] = {0};
                if (GetVolumeInformationW(path.toStdWString().c_str(),
                                        nullptr, 0, nullptr, nullptr, nullptr,
                                        fsName, MAX_PATH)) {
                    QString fs = QString::fromWCharArray(fsName);
                    if (fs == "FAT32") dev.fileSystem = FileSystem::FAT32;
                    else if (fs == "NTFS") dev.fileSystem = FileSystem::NTFS;
                    else if (fs == "exFAT") dev.fileSystem = FileSystem::exFAT;
                }
            }

            dev.status = DeviceStatus::Ready;
            devices.append(dev);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
#endif

    return devices;
}

QString DeviceEnumerator::getDeviceInstanceId(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData) {
    Q_UNUSED(hDevInfo);
    WCHAR instanceId[MAX_PATH];
    if (CM_Get_Device_IDW(devInfoData.DevInst, instanceId, MAX_PATH, 0) == CR_SUCCESS) {
        return QString::fromWCharArray(instanceId);
    }
    return QString();
}

QString DeviceEnumerator::getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData,
                                            DWORD property) {
    DWORD size = 256;
    WCHAR buffer[256] = {0};

    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, property,
                                          nullptr, reinterpret_cast<PBYTE>(buffer),
                                          size, &size)) {
        return QString::fromWCharArray(buffer);
    }
    return QString();
}

int DeviceEnumerator::getDriveLetterFromDeviceId(const QString& deviceId) {
    Q_UNUSED(deviceId);
    return -1;
}