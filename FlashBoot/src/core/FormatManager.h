#ifndef FORMATMANGER_H
#define FORMATMANGER_H

#include "pch.h"
#include "UsbDevice.h"
#include "IsoInfo.h"

enum class FormatStage {
    Idle,
    Preparing,
    DownloadingFiles,
    WritingMBR,
    WritingPBR,
    CopyingFiles,
    InstallingDrivers,
    Finalizing,
    Completed,
    Cancelled,
    Error
};

class FormatManager : public QObject {
    Q_OBJECT

public:
    explicit FormatManager(QObject* parent = nullptr);
    ~FormatManager();

    struct FormatOptions {
        PartitionScheme partitionScheme = PartitionScheme::GPT;
        FileSystem fileSystem = FileSystem::FAT32;
        QString volumeLabel = "FLASHBOOT";
        bool quickFormat = true;
        bool checkBadBlocks = false;
        bool createExtendedLabel = true;
        bool useFixedDriveLetter = false;
    };

    void startCreation(const UsbDevice& device, const IsoInfo& iso, 
                       const FormatOptions& options);
    void cancel();
    bool isRunning() const;

signals:
    void stageChanged(FormatStage stage, const QString& description);
    void progressChanged(int percent, const QString& currentFile);
    void speedUpdated(double mbPerSecond);
    void timeUpdated(int elapsedSeconds, int remainingSeconds);
    void finished(bool success, const QString& message);
    void errorOccurred(const QString& error);
    void logMessage(const QString& msg);

private:
    void runCreation(const UsbDevice& device, const IsoInfo& iso, 
                     const FormatOptions& options);
    bool formatDevice(const UsbDevice& device, const FormatOptions& options);

    std::atomic<bool> m_cancelled{false};
    std::atomic<bool> m_running{false};
    QThread* m_workerThread = nullptr;

    QElapsedTimer m_timer;
    qint64 m_bytesWritten = 0;
};

#endif // FORMATMANGER_H