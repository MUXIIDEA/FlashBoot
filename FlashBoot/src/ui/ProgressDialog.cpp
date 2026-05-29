#include "ProgressDialog.h"

ProgressDialog::ProgressDialog(QWidget* parent) : QDialog(parent) {
    setupUI();
}

void ProgressDialog::setupUI() {
    setWindowTitle("正在创建启动盘...");
    setFixedSize(500, 400);
    setModal(true);

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);

    m_deviceLabel = new QLabel("设备: -", this);
    m_imageLabel = new QLabel("镜像: -", this);
    m_modeLabel = new QLabel("模式: -", this);
    layout->addWidget(m_deviceLabel);
    layout->addWidget(m_imageLabel);
    layout->addWidget(m_modeLabel);

    auto* progressGroup = new QGroupBox("进度", this);
    auto* progressLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    progressLayout->addWidget(m_progressBar);

    m_progressLabel = new QLabel("准备就绪...", this);
    m_progressLabel->setStyleSheet("text-align: center; font-weight: bold;");
    progressLayout->addWidget(m_progressLabel);
    layout->addWidget(progressGroup);

    auto* infoLayout = new QHBoxLayout();
    m_timeLabel = new QLabel("已用时间: 00:00 | 剩余时间: --:--", this);
    m_timeLabel->setStyleSheet("font-size: 11px;");
    infoLayout->addWidget(m_timeLabel);

    m_speedLabel = new QLabel("速度: 0 MB/s", this);
    m_speedLabel->setStyleSheet("font-size: 11px;");
    infoLayout->addWidget(m_speedLabel);
    layout->addLayout(infoLayout);

    auto* detailsGroup = new QGroupBox("详细进度", this);
    auto* detailsLayout = new QVBoxLayout(detailsGroup);

    m_detailsList = new QListWidget(this);
    m_detailsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    detailsLayout->addWidget(m_detailsList);
    layout->addWidget(detailsGroup);

    auto* buttonLayout = new QHBoxLayout();
    m_pauseBtn = new QPushButton("⏸ 暂停", this);
    m_pauseBtn->setStyleSheet("padding: 6px 16px;");
    connect(m_pauseBtn, &QPushButton::clicked, this, &ProgressDialog::onPauseClicked);
    buttonLayout->addWidget(m_pauseBtn);

    m_cancelBtn = new QPushButton("⏹ 取消", this);
    m_cancelBtn->setStyleSheet("padding: 6px 16px; background-color: #D13438; color: white; border: none; border-radius: 4px;");
    connect(m_cancelBtn, &QPushButton::clicked, this, &ProgressDialog::onCancelClicked);
    buttonLayout->addWidget(m_cancelBtn);

    buttonLayout->addStretch(1);
    layout->addLayout(buttonLayout);

    auto* warningLabel = new QLabel("⚠️ 请勿移除设备或关闭软件!", this);
    warningLabel->setStyleSheet("color: #D13438; text-align: center; font-size: 11px;");
    layout->addWidget(warningLabel);

    setLayout(layout);
}

void ProgressDialog::setDevice(const QString& device) {
    m_deviceLabel->setText(QString("设备: %1").arg(device));
}

void ProgressDialog::setImage(const QString& image) {
    m_imageLabel->setText(QString("镜像: %1").arg(image));
}

void ProgressDialog::setMode(const QString& mode) {
    m_modeLabel->setText(QString("模式: %1").arg(mode));
}

void ProgressDialog::setProgress(int percent, const QString& status) {
    m_progressBar->setValue(percent);
    m_progressLabel->setText(QString("%1% - %2").arg(percent).arg(status));
}

void ProgressDialog::setTimeInfo(int elapsed, int remaining) {
    QString elapsedStr = QString("%1:%2").arg(elapsed / 60, 2, 10, QChar('0')).arg(elapsed % 60, 2, 10, QChar('0'));
    QString remainingStr = remaining >= 0 ? QString("%1:%2").arg(remaining / 60, 2, 10, QChar('0')).arg(remaining % 60, 2, 10, QChar('0')) : "--:--";
    m_timeLabel->setText(QString("已用时间: %1 | 剩余时间: %2").arg(elapsedStr).arg(remainingStr));
}

void ProgressDialog::setSpeed(double mbps) {
    m_speedLabel->setText(QString("速度: %1 MB/s").arg(mbps, 0, 'f', 1));
}

void ProgressDialog::addProgressItem(const QString& text, bool done) {
    auto* item = new QListWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    if (done) {
        item->setText("[✓] " + text);
        item->setForeground(QColor("#54B054"));
    } else {
        item->setText("[ ] " + text);
    }
    m_detailsList->addItem(item);
}

void ProgressDialog::updateProgressItem(int index, bool done) {
    if (index >= 0 && index < m_detailsList->count()) {
        auto* item = m_detailsList->item(index);
        QString text = item->text();
        text = text.mid(4);
        if (done) {
            item->setText("[✓] " + text);
            item->setForeground(QColor("#54B054"));
        } else {
            item->setText("[→] " + text);
            item->setForeground(QColor("#0078D4"));
        }
    }
}

void ProgressDialog::onPauseClicked() {
    emit paused();
}

void ProgressDialog::onCancelClicked() {
    emit cancelled();
}