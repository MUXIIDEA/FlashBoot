#ifndef DEVICEPANEL_H
#define DEVICEPANEL_H

#include "pch.h"
#include "UsbDevice.h"

class CapacityBar;

class DevicePanel : public QWidget {
    Q_OBJECT

public:
    explicit DevicePanel(QWidget* parent = nullptr);

    void setDevices(const QList<UsbDevice>& devices);
    void updateDevice(const UsbDevice& device);
    void removeDevice(const QString& deviceId);
    UsbDevice selectedDevice() const;

signals:
    void deviceSelected(const UsbDevice& device);
    void refreshRequested();

private slots:
    void onDeviceClicked(QListWidgetItem* item);
    void onRefreshClicked();

private:
    void setupUI();

    QListWidget* m_deviceList;
    QLabel* m_infoDevice;
    QLabel* m_infoModel;
    QLabel* m_infoCapacity;
    QLabel* m_infoFileSystem;
    QLabel* m_infoPartition;
    QComboBox* m_fsCombo;
    QComboBox* m_clusterCombo;
    QLineEdit* m_labelEdit;
    QPushButton* m_refreshBtn;

    QList<UsbDevice> m_devices;
};

#endif // DEVICEPANEL_H