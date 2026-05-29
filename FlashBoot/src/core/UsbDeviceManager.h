#ifndef USBDEVICEMANAGER_H
#define USBDEVICEMANAGER_H

#include "pch.h"
#include "UsbDevice.h"

class UsbDeviceManager : public QObject {
    Q_OBJECT

public:
    explicit UsbDeviceManager(QObject* parent = nullptr);
    ~UsbDeviceManager();

    void startMonitoring();
    void stopMonitoring();
    void refreshDevices();

    QList<UsbDevice> devices() const;
    UsbDevice deviceById(const QString& id) const;

signals:
    void devicesChanged(const QList<UsbDevice>& devices);
    void deviceAdded(const UsbDevice& device);
    void deviceRemoved(const QString& deviceId);
    void deviceUpdated(const UsbDevice& device);
    void errorOccurred(const QString& error);

private slots:
    void onDeviceArrival(const QString& devicePath);
    void onDeviceRemove(const QString& devicePath);

private:
    QList<UsbDevice> enumerateUsbDrives();
    UsbDevice parseDeviceInfo(const QString& devicePath);

    QList<UsbDevice> m_devices;
    mutable QMutex m_mutex;

#ifdef Q_OS_WIN
    HDEVNOTIFY m_devNotify = nullptr;
    static LRESULT CALLBACK deviceWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void registerDeviceNotification();
    void unregisterDeviceNotification();
#endif
};

#endif // USBDEVICEMANAGER_H