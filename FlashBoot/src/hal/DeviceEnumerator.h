#ifndef DEVICEENUMERATOR_H
#define DEVICEENUMERATOR_H

#include "pch.h"
#include "UsbDevice.h"

class DeviceEnumerator {
public:
    static QList<UsbDevice> enumerateUsbDevices();
    static QString getDeviceInstanceId(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData);
    static QString getDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA& devInfoData, 
                                     DWORD property);
    static int getDriveLetterFromDeviceId(const QString& deviceId);
};

#endif // DEVICEENUMERATOR_H