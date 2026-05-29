#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "pch.h"
#include "UsbDevice.h"

class CapacityBar;

class DeviceItem : public QWidget {
    Q_OBJECT
public:
    explicit DeviceItem(const UsbDevice& device, QWidget* parent = nullptr);
    UsbDevice device() const { return m_device; }
    void updateDevice(const UsbDevice& device);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    UsbDevice m_device;
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    CapacityBar* m_capacityBar;
    QLabel* m_capacityLabel;
};

#endif // DEVICEITEM_H