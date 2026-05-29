#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "pch.h"

class ProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget* parent = nullptr);

public slots:
    void setDevice(const QString& device);
    void setImage(const QString& image);
    void setMode(const QString& mode);
    void setProgress(int percent, const QString& status);
    void setTimeInfo(int elapsed, int remaining);
    void setSpeed(double mbps);
    void addProgressItem(const QString& text, bool done = false);
    void updateProgressItem(int index, bool done);

signals:
    void paused();
    void cancelled();

private slots:
    void onPauseClicked();
    void onCancelClicked();

private:
    void setupUI();

    QLabel* m_deviceLabel;
    QLabel* m_imageLabel;
    QLabel* m_modeLabel;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    QLabel* m_timeLabel;
    QLabel* m_speedLabel;
    QListWidget* m_detailsList;
    QPushButton* m_pauseBtn;
    QPushButton* m_cancelBtn;
};

#endif // PROGRESSDIALOG_H