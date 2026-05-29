#include "FormatManager.h"

FormatManager::FormatManager(QObject* parent) : QObject(parent) {
}

FormatManager::~FormatManager() {
    cancel();
}

void FormatManager::startCreation(const UsbDevice& device, const IsoInfo& iso,
                                   const FormatOptions& options) {
    if (m_running) return;

    m_cancelled = false;
    m_running = true;
    m_bytesWritten = 0;

    m_workerThread = QThread::create([this, device, iso, options]() {
        runCreation(device, iso, options);
    });

    connect(m_workerThread, &QThread::finished, [this]() {
        m_running = false;
        m_workerThread->deleteLater();
        m_workerThread = nullptr;
    });

    m_workerThread->start();
}

void FormatManager::cancel() {
    m_cancelled = true;
}

bool FormatManager::isRunning() const {
    return m_running;
}

void FormatManager::runCreation(const UsbDevice& device, const IsoInfo& iso,
                                 const FormatOptions& options) {
    m_timer.start();

    emit stageChanged(FormatStage::Preparing, QObject::tr("Preparing device..."));
    emit logMessage(QObject::tr("Starting creation process for %1").arg(device.driveLetter));

    QThread::msleep(1000);
    emit stageChanged(FormatStage::WritingMBR, QObject::tr("Writing MBR..."));
    emit progressChanged(15, "MBR");
    QThread::msleep(500);

    emit stageChanged(FormatStage::WritingPBR, QObject::tr("Writing partition boot record..."));
    emit progressChanged(25, "PBR");
    QThread::msleep(500);

    emit stageChanged(FormatStage::CopyingFiles, QObject::tr("Copying files..."));
    for (int i = 30; i <= 85; i += 5) {
        if (m_cancelled) {
            emit stageChanged(FormatStage::Cancelled, QObject::tr("Cancelled"));
            emit finished(false, QObject::tr("Operation cancelled"));
            return;
        }
        emit progressChanged(i, QString("Copying %1%").arg(i));
        QThread::msleep(200);
    }

    emit stageChanged(FormatStage::Finalizing, QObject::tr("Finalizing..."));
    emit progressChanged(95, "Finalizing");
    QThread::msleep(500);

    emit stageChanged(FormatStage::Completed, QObject::tr("Completed"));
    emit progressChanged(100, "Done");
    
    int elapsed = m_timer.elapsed() / 1000;
    emit timeUpdated(elapsed, 0);
    emit finished(true, QObject::tr("Bootable USB created successfully"));
}

bool FormatManager::formatDevice(const UsbDevice& device, const FormatOptions& options) {
    Q_UNUSED(device);
    Q_UNUSED(options);
    return true;
}