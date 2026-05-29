#include "DeviceItem.h"
#include "CapacityBar.h"

DeviceItem::DeviceItem(const UsbDevice& device, QWidget* parent)
    : QWidget(parent), m_device(device) {

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 6, 8, 6);

    auto* topLayout = new QHBoxLayout();
    m_iconLabel = new QLabel(this);
    QIcon icon(":/icons/usb.png");
    m_iconLabel->setPixmap(icon.pixmap(24, 24));
    topLayout->addWidget(m_iconLabel);

    QString titleText = QString("%1 %2").arg(device.driveLetter).arg(device.product);
    if (titleText.trimmed().isEmpty()) {
        titleText = QString("%1 (USB Device)").arg(device.driveLetter);
    }
    m_titleLabel = new QLabel(titleText, this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 11px;");
    topLayout->addWidget(m_titleLabel, 1);
    layout->addLayout(topLayout);

    auto* capLayout = new QHBoxLayout();
    m_capacityBar = new CapacityBar(this);
    m_capacityBar->setValue(device.usedPercent());
    m_capacityBar->setFixedHeight(6);
    capLayout->addWidget(m_capacityBar, 1);

    m_capacityLabel = new QLabel(UsbDevice::formatBytes(device.totalBytes), this);
    m_capacityLabel->setStyleSheet("color: gray; font-size: 9px;");
    capLayout->addWidget(m_capacityLabel);
    layout->addLayout(capLayout);

    setLayout(layout);
}

void DeviceItem::updateDevice(const UsbDevice& device) {
    m_device = device;

    QString titleText = QString("%1 %2").arg(device.driveLetter).arg(device.product);
    if (titleText.trimmed().isEmpty()) {
        titleText = QString("%1 (USB Device)").arg(device.driveLetter);
    }
    m_titleLabel->setText(titleText);

    m_capacityBar->setValue(device.usedPercent());
    m_capacityLabel->setText(UsbDevice::formatBytes(device.totalBytes));

    update();
}

void DeviceItem::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(QColor(230, 230, 230));
    painter.drawLine(0, height() - 1, width(), height() - 1);
}