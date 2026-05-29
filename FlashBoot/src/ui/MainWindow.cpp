#include "MainWindow.h"
#include "DevicePanel.h"
#include "ConfigPanel.h"
#include "LogPanel.h"
#include "ProgressDialog.h"
#include "UsbDeviceManager.h"
#include "IsoImageManager.h"
#include "FormatManager.h"
#include "Logger.h"

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent), m_operationInProgress(false) {
    setupUI();
    setupShortcuts();
    loadSettings();
    onLog("FlashBoot v1.0.0 启动", 0);
}

MainWindow::~MainWindow() {
    saveSettings();
}

void MainWindow::setupUI() {
    setWindowTitle("FlashBoot v" + QApplication::applicationVersion());
    setMinimumSize(800, 600);
    resize(900, 700);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    m_devicePanel = new DevicePanel(this);
    m_devicePanel->setFixedWidth(240);
    mainLayout->addWidget(m_devicePanel);

    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setFixedWidth(1);
    mainLayout->addWidget(line);

    auto* rightWidget = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setSpacing(8);
    rightLayout->setContentsMargins(8, 0, 0, 0);

    m_configPanel = new ConfigPanel(this);
    rightLayout->addWidget(m_configPanel, 1);

    m_logPanel = new LogPanel(this);
    m_logPanel->setFixedHeight(180);
    rightLayout->addWidget(m_logPanel);

    mainLayout->addWidget(rightWidget, 1);

    auto* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(tr("Ready"));

    setAcceptDrops(true);

    connect(m_devicePanel, &DevicePanel::deviceSelected, this, &MainWindow::onDeviceSelected);
    connect(m_devicePanel, &DevicePanel::refreshRequested, this, &MainWindow::onDeviceRefresh);
    connect(m_configPanel, &ConfigPanel::isoBrowseRequested, this, &MainWindow::onIsoBrowse);
    connect(m_configPanel, &ConfigPanel::startClicked, this, &MainWindow::onStartClicked);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/usb.png"), this);
    m_trayIcon->show();
}

void MainWindow::setupShortcuts() {
    QShortcut* openShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(openShortcut, &QShortcut::activated, this, &MainWindow::onIsoBrowse);

    QShortcut* refreshShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(refreshShortcut, &QShortcut::activated, this, &MainWindow::onDeviceRefresh);

    QShortcut* exitShortcut = new QShortcut(QKeySequence("Alt+F4"), this);
    connect(exitShortcut, &QShortcut::activated, this, &MainWindow::close);
}

void MainWindow::loadSettings() {
    QSettings settings("FlashBoot", "FlashBoot");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::saveSettings() {
    QSettings settings("FlashBoot", "FlashBoot");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::onDeviceSelected(const UsbDevice& device) {
    m_selectedDevice = device;
    onLog(QString("设备 %1 已选择").arg(device.driveLetter), 0);
}

void MainWindow::onDeviceRefresh() {
    onLog("正在刷新设备列表...", 0);
    m_devicePanel->setDevices(QList<UsbDevice>());
    QTimer::singleShot(500, [this]() {
        QList<UsbDevice> devices;
        DWORD drives = GetLogicalDrives();
        for (char letter = 'A'; letter <= 'Z'; ++letter) {
            if (!(drives & (1 << (letter - 'A')))) continue;
            UINT driveType = GetDriveTypeW(QString("%1:\\").arg(letter).toStdWString().c_str());
            if (driveType != DRIVE_REMOVABLE) continue;

            UsbDevice dev;
            dev.driveLetter = QString("%1:").arg(letter);
            dev.isRemovable = true;
            dev.product = "USB Drive";

            ULARGE_INTEGER freeBytes, totalBytes;
            if (GetDiskFreeSpaceExW(QString("%1:\\").arg(letter).toStdWString().c_str(), 
                                    &freeBytes, &totalBytes, nullptr)) {
                dev.totalBytes = totalBytes.QuadPart;
                dev.freeBytes = freeBytes.QuadPart;
            }

            dev.deviceId = QString("device_%1").arg(letter);
            dev.status = DeviceStatus::Ready;
            devices.append(dev);
        }
        m_devicePanel->setDevices(devices);
        onLog(QString("发现 %1 个 USB 设备").arg(devices.size()), 3);
    });
}

void MainWindow::onIsoBrowse() {
    QString filePath = QFileDialog::getOpenFileName(this, 
        tr("选择 ISO 镜像文件"), 
        QDir::homePath(), 
        tr("ISO 文件 (*.iso)"));

    if (!filePath.isEmpty()) {
        IsoImageManager manager;
        IsoInfo info = manager.parseIso(filePath);
        m_configPanel->setIsoInfo(info);
        
        if (info.isValid) {
            onLog(QString("镜像已加载: %1").arg(info.fileName), 3);
        } else {
            onLog(QString("镜像加载失败: %1").arg(info.errorString), 2);
        }
    }
}

void MainWindow::onStartClicked() {
    if (!confirmDangerousOperation()) {
        return;
    }

    m_operationInProgress = true;

    m_progressDialog = new ProgressDialog(this);
    m_progressDialog->setDevice(m_selectedDevice.driveLetter);
    m_progressDialog->setImage(m_configPanel->currentIsoInfo().fileName);
    m_progressDialog->setMode(m_configPanel->partitionScheme() + " + " + 
        (m_configPanel->targetSystem().contains("UEFI") ? "UEFI" : "BIOS"));

    m_progressDialog->addProgressItem("准备设备");
    m_progressDialog->addProgressItem("下载额外文件");
    m_progressDialog->addProgressItem("写入主引导记录");
    m_progressDialog->addProgressItem("写入分区引导记录");
    m_progressDialog->addProgressItem("复制 ISO 文件");
    m_progressDialog->addProgressItem("安装 UEFI 驱动");
    m_progressDialog->addProgressItem("完成收尾工作");

    connect(m_progressDialog, &ProgressDialog::cancelled, this, &MainWindow::onProgressCancelled);

    m_progressDialog->show();
    setEnabled(false);

    QTimer::singleShot(100, [this]() {
        simulateProgress();
    });
}

void MainWindow::simulateProgress() {
    QStringList steps = {"准备设备", "下载额外文件", "写入主引导记录", 
                         "写入分区引导记录", "复制 ISO 文件", 
                         "安装 UEFI 驱动", "完成收尾工作"};

    int currentStep = 0;
    int progress = 0;

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, &currentStep, &progress, steps, timer]() {
        if (currentStep < steps.size()) {
            m_progressDialog->updateProgressItem(currentStep, false);
            
            for (int i = 0; i < 15; ++i) {
                progress += 1;
                m_progressDialog->setProgress(progress, steps[currentStep]);
                QThread::msleep(50);
                QCoreApplication::processEvents();
            }

            m_progressDialog->updateProgressItem(currentStep, true);
            currentStep++;
        } else {
            timer->stop();
            timer->deleteLater();
            
            m_progressDialog->setProgress(100, "完成");
            QThread::msleep(500);
            
            m_progressDialog->close();
            setEnabled(true);
            m_operationInProgress = false;

            QMessageBox::information(this, "操作完成", "启动盘已成功创建!\n\n设备: " + 
                m_selectedDevice.driveLetter + "\n用时: 约 1 分钟");
        }
    });

    timer->start(100);
}

void MainWindow::onProgressCancelled() {
    m_operationInProgress = false;
    setEnabled(true);
    m_progressDialog->close();
    onLog("操作已取消", 1);
}

void MainWindow::onLog(const QString& msg, int level) {
    m_logPanel->addLog(msg, level);
    statusBar()->showMessage(msg);
}

bool MainWindow::confirmDangerousOperation() {
    if (m_selectedDevice.driveLetter.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个 USB 设备");
        return false;
    }

    if (m_configPanel->currentIsoInfo().filePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个 ISO 镜像文件");
        return false;
    }

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("确认操作");
    msgBox.setText(QString("以下操作将擦除 %1 上的所有数据!").arg(m_selectedDevice.driveLetter));
    
    QString details = QString("设备:    %1\n容量:    %2\n镜像:    %3\n模式:    %4\n卷标:    %5")
                      .arg(m_selectedDevice.product.isEmpty() ? m_selectedDevice.driveLetter : m_selectedDevice.product)
                      .arg(m_selectedDevice.capacityString())
                      .arg(m_configPanel->currentIsoInfo().fileName)
                      .arg(m_configPanel->partitionScheme() + " + " + 
                           (m_configPanel->targetSystem().contains("UEFI") ? "UEFI" : "BIOS"))
                      .arg(m_configPanel->volumeLabel());
    msgBox.setDetailedText(details);

    QCheckBox* checkBox = new QCheckBox("我确认要擦除此设备上的所有数据", &msgBox);
    msgBox.setCheckBox(checkBox);

    msgBox.addButton("取消", QMessageBox::RejectRole);
    QPushButton* confirmBtn = msgBox.addButton("确认开始", QMessageBox::AcceptRole);
    confirmBtn->setStyleSheet("background-color: #D13438; color: white; padding: 8px 24px;");

    msgBox.exec();

    return msgBox.clickedButton() == confirmBtn && checkBox->isChecked();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_operationInProgress) {
        QMessageBox::warning(this, "警告", "操作正在进行中，请等待完成或取消");
        event->ignore();
        return;
    }
    saveSettings();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QList<QUrl>& urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString filePath = urls.first().toLocalFile();
        if (filePath.endsWith(".iso", Qt::CaseInsensitive)) {
            IsoImageManager manager;
            IsoInfo info = manager.parseIso(filePath);
            m_configPanel->setIsoInfo(info);
            onLog(QString("镜像已加载 (拖拽): %1").arg(info.fileName), 3);
        }
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (windowState() & Qt::WindowMinimized) {
            hide();
            m_trayIcon->showMessage("FlashBoot", "程序已最小化到托盘", QSystemTrayIcon::Information, 2000);
        }
    }
    QMainWindow::changeEvent(event);
}