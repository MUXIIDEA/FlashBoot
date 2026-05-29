#include "LogPanel.h"
#include <QScrollBar>

LogPanel::LogPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void LogPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* headerLayout = new QHBoxLayout();
    auto* statusLabel = new QLabel("状态: 准备就绪", this);
    statusLabel->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(statusLabel);

    m_copyBtn = new QPushButton("📋 复制日志", this);
    m_copyBtn->setStyleSheet("padding: 2px 8px; font-size: 11px;");
    connect(m_copyBtn, &QPushButton::clicked, this, &LogPanel::onCopyClicked);
    headerLayout->addWidget(m_copyBtn);

    headerLayout->addStretch(1);
    layout->addLayout(headerLayout);

    m_logEdit = new QPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setFont(QFont("Consolas", 9));
    m_logEdit->setStyleSheet("background-color: #1E1E1E; color: #D4D4D4;");
    layout->addWidget(m_logEdit, 1);

    setLayout(layout);
}

void LogPanel::addLog(const QString& message, int level) {
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
    QString color;
    switch (level) {
        case 1: color = "#FFB900"; break;
        case 2: color = "#FF5252"; break;
        case 3: color = "#54B054"; break;
        default: color = "#D4D4D4";
    }

    QString htmlMessage = QString("<span style='color:%1;'>%2</span><br>")
                          .arg(color)
                          .arg(timestamp + message);
    m_logEdit->appendHtml(htmlMessage);
    m_logEdit->verticalScrollBar()->setValue(m_logEdit->verticalScrollBar()->maximum());
}

void LogPanel::clearLogs() {
    m_logEdit->clear();
}

QString LogPanel::getLogText() const {
    return m_logEdit->toPlainText();
}

void LogPanel::onCopyClicked() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(getLogText());
    emit copyLogRequested();
}