#include "UsbDeviceManager.h"

UsbDeviceManager::UsbDeviceManager(QObject* parent) : QObject(parent) {
}

UsbDeviceManager::~UsbDeviceManager() {
    stopMonitoring();
}

void UsbDeviceManager::startMonitoring() {
#ifdef Q_OS_WIN
    registerDeviceNotification();
#endif
    refreshDevices();
}

void UsbDeviceManager::stopMonitoring() {
#ifdef Q_OS_WIN
    unregisterDeviceNotification();
#endif
}

void UsbDeviceManager::refreshDevices() {
    QMutexLocker locker(&m_mutex);
    m_devices = enumerateUsbDrives();
    emit devicesChanged(m_devices);
}

QList<UsbDevice> UsbDeviceManager::devices() const {
    QMutexLocker locker(&m_mutex);
    return m_devices;
}

UsbDevice UsbDeviceManager::deviceById(const QString& id) const {
    QMutexLocker locker(&m_mutex);
    for (const auto& dev : m_devices) {
        if (dev.deviceId == id) {
            return dev;
        }
    }
    return UsbDevice();
}

void UsbDeviceManager::onDeviceArrival(const QString& devicePath) {
    Q_UNUSED(devicePath);
    refreshDevices();
}

void UsbDeviceManager::onDeviceRemove(const QString& devicePath) {
    Q_UNUSED(devicePath);
    refreshDevices();
}

QList<UsbDevice> UsbDeviceManager::enumerateUsbDrives() {
    QList<UsbDevice> result;

#ifdef Q_OS_WIN
    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (!(drives & (1 << (letter - 'A')))) continue;

        QString drivePath = QString("%1:\\").arg(letter);
        UINT driveType = GetDriveTypeW(drivePath.toStdWString().c_str());

        if (driveType != DRIVE_REMOVABLE && driveType != DRIVE_FIXED) continue;

        UsbDevice dev;
        dev.driveLetter = QString("%1:").arg(letter);
        dev.isRemovable = (driveType == DRIVE_REMOVABLE);

        ULARGE_INTEGER freeBytes, totalBytes;
        if (GetDiskFreeSpaceExW(drivePath.toStdWString().c_str(), 
                                &freeBytes, &totalBytes, nullptr)) {
            dev.totalBytes = totalBytes.QuadPart;
            dev.freeBytes = freeBytes.QuadPart;
        }

        WCHAR volumeName[MAX_PATH + 1] = {0};
        WCHAR fsName[MAX_PATH + 1] = {0};
        if (GetVolumeInformationW(drivePath.toStdWString().c_str(),
                                volumeName, MAX_PATH,
                                nullptr, nullptr, nullptr,
                                fsName, MAX_PATH)) {
            QString fs = QString::fromWCharArray(fsName);
            if (fs == "FAT32") dev.fileSystem = FileSystem::FAT32;
            else if (fs == "NTFS") dev.fileSystem = FileSystem::NTFS;
            else if (fs == "exFAT") dev.fileSystem = FileSystem::exFAT;
            else if (fs == "UDF") dev.fileSystem = FileSystem::UDF;
        }

        dev.deviceId = QString("device_%1").arg(letter);
        dev.product = QString("USB Drive %1").arg(letter);
        dev.status = DeviceStatus::Ready;

        result.append(dev);
    }
#endif

    return result;
}

UsbDevice UsbDeviceManager::parseDeviceInfo(const QString& devicePath) {
    UsbDevice dev;
    dev.deviceId = devicePath;
    dev.status = DeviceStatus::Ready;
    return dev;
}

#ifdef Q_OS_WIN
LRESULT CALLBACK UsbDeviceManager::deviceWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DEVICECHANGE) {
        UsbDeviceManager* manager = reinterpret_cast<UsbDeviceManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (manager) {
            PDEV_BROADCAST_HDR hdr = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
            if (hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME volume = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lParam);
                if (wParam == DBT_DEVICEARRIVAL) {
                    manager->onDeviceArrival(QString::number(volume->dbcv_unitmask));
                } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                    manager->onDeviceRemove(QString::number(volume->dbcv_unitmask));
                }
            }
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void UsbDeviceManager::registerDeviceNotification() {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = deviceWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"FlashBootDeviceNotification";
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, L"FlashBootDeviceNotification", L"", 0, 0, 0, 0, 0, 
                                HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    DEV_BROADCAST_VOLUME filter = {0};
    filter.dbcv_size = sizeof(DEV_BROADCAST_VOLUME);
    filter.dbcv_devicetype = DBT_DEVTYP_VOLUME;
    filter.dbcv_reserved = 0;
    filter.dbcv_unitmask = 0;
    filter.dbcv_flags = DBTF_MEDIA;

    m_devNotify = RegisterDeviceNotification(hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void UsbDeviceManager::unregisterDeviceNotification() {
    if (m_devNotify) {
        UnregisterDeviceNotification(m_devNotify);
        m_devNotify = nullptr;
    }
}
#endif