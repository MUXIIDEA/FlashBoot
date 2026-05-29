#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pch.h"
#include "UsbDevice.h"

class DevicePanel;
class ConfigPanel;
class LogPanel;
class ProgressDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onDeviceSelected(const UsbDevice& device);
    void onDeviceRefresh();
    void onIsoBrowse();
    void onStartClicked();
    void onProgressCancelled();
    void onLog(const QString& msg, int level);

private:
    void setupUI();
    void setupShortcuts();
    void loadSettings();
    void saveSettings();
    bool confirmDangerousOperation();
    void simulateProgress();

    DevicePanel* m_devicePanel;
    ConfigPanel* m_configPanel;
    LogPanel* m_logPanel;
    ProgressDialog* m_progressDialog;

    UsbDevice m_selectedDevice;
    QSystemTrayIcon* m_trayIcon;
    bool m_operationInProgress;
};

#endif // MAINWINDOW_H