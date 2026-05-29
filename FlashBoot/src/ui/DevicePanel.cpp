#include "DevicePanel.h"
#include "widgets/DeviceItem.h"

DevicePanel::DevicePanel(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void DevicePanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("USB 设备", this);
    label->setStyleSheet("font-weight: bold; font-size: 14px;");
    headerLayout->addWidget(label);

    m_refreshBtn = new QPushButton(this);
    m_refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    m_refreshBtn->setStyleSheet("padding: 4px;");
    m_refreshBtn->setToolTip("刷新设备列表");
    connect(m_refreshBtn, &QPushButton::clicked, this, &DevicePanel::onRefreshClicked);
    headerLayout->addWidget(m_refreshBtn);

    headerLayout->addStretch(1);
    layout->addWidget(header);

    m_deviceList = new QListWidget(this);
    m_deviceList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_deviceList->setStyleSheet("QListWidget { border: 1px solid #DDD; border-radius: 4px; }");
    connect(m_deviceList, &QListWidget::itemClicked, this, &DevicePanel::onDeviceClicked);
    layout->addWidget(m_deviceList, 1);

    auto* infoGroup = new QGroupBox("设备信息", this);
    auto* infoLayout = new QFormLayout(infoGroup);

    m_infoDevice = new QLabel("-", this);
    m_infoModel = new QLabel("-", this);
    m_infoCapacity = new QLabel("-", this);
    m_infoFileSystem = new QLabel("-", this);
    m_infoPartition = new QLabel("-", this);

    infoLayout->addRow("设备:", m_infoDevice);
    infoLayout->addRow("型号:", m_infoModel);
    infoLayout->addRow("容量:", m_infoCapacity);
    infoLayout->addRow("文件系统:", m_infoFileSystem);
    infoLayout->addRow("分区:", m_infoPartition);

    layout->addWidget(infoGroup);

    setLayout(layout);
}

void DevicePanel::setDevices(const QList<UsbDevice>& devices) {
    m_devices = devices;
    m_deviceList->clear();

    for (const UsbDevice& dev : devices) {
        auto* item = new QListWidgetItem();
        item->setData(Qt::UserRole, QVariant::fromValue(dev));

        DeviceItem* widget = new DeviceItem(dev);
        item->setSizeHint(widget->sizeHint());

        m_deviceList->addItem(item);
        m_deviceList->setItemWidget(item, widget);
    }
}

void DevicePanel::updateDevice(const UsbDevice& device) {
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == device.deviceId) {
            m_devices[i] = device;

            QListWidgetItem* item = m_deviceList->item(i);
            if (item) {
                DeviceItem* widget = qobject_cast<DeviceItem*>(m_deviceList->itemWidget(item));
                if (widget) {
                    widget->updateDevice(device);
                    item->setSizeHint(widget->sizeHint());
                }
            }
            break;
        }
    }
}

void DevicePanel::removeDevice(const QString& deviceId) {
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices[i].deviceId == deviceId) {
            m_devices.removeAt(i);
            delete m_deviceList->takeItem(i);
            break;
        }
    }
}

UsbDevice DevicePanel::selectedDevice() const {
    QListWidgetItem* item = m_deviceList->currentItem();
    if (item) {
        return item->data(Qt::UserRole).value<UsbDevice>();
    }
    return UsbDevice();
}

void DevicePanel::onDeviceClicked(QListWidgetItem* item) {
    if (item) {
        UsbDevice device = item->data(Qt::UserRole).value<UsbDevice>();
        emit deviceSelected(device);

        m_infoDevice->setText(device.driveLetter.isEmpty() ? "-" : device.driveLetter);
        m_infoModel->setText(device.product.isEmpty() ? "-" : device.product);
        m_infoCapacity->setText(device.capacityString());
        m_infoFileSystem->setText(device.fileSystemString());
        m_infoPartition->setText(device.partitionSchemeString());
    }
}

void DevicePanel::onRefreshClicked() {
    emit refreshRequested();
}