#ifndef LOGPANEL_H
#define LOGPANEL_H

#include "pch.h"

class LogPanel : public QWidget {
    Q_OBJECT

public:
    explicit LogPanel(QWidget* parent = nullptr);

    void addLog(const QString& message, int level = 0);
    void clearLogs();
    QString getLogText() const;

signals:
    void copyLogRequested();

private slots:
    void onCopyClicked();

private:
    void setupUI();

    QPlainTextEdit* m_logEdit;
    QPushButton* m_copyBtn;
};

#endif // LOGPANEL_H