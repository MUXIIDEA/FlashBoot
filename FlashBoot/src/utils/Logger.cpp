#include "Logger.h"

Logger* Logger::m_instance = nullptr;

Logger::Logger(QObject* parent) : QObject(parent) {
}

Logger::~Logger() {
}

Logger* Logger::instance() {
    if (!m_instance) {
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::log(const QString& message, LogLevel level) {
    QMutexLocker locker(&m_mutex);
    QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    QString levelStr;
    switch (level) {
        case Info: levelStr = "[INFO]"; break;
        case Warning: levelStr = "[WARN]"; break;
        case Error: levelStr = "[ERROR]"; break;
        case Success: levelStr = "[OK]"; break;
    }
    QString logEntry = QString("%1 %2 %3").arg(timestamp).arg(levelStr).arg(message);
    m_logBuffer.append(logEntry);
    
    if (m_logBuffer.size() > 1000) {
        m_logBuffer.removeFirst();
    }

    emit newLog(message, level);
}

void Logger::info(const QString& message) {
    log(message, Info);
}

void Logger::warning(const QString& message) {
    log(message, Warning);
}

void Logger::error(const QString& message) {
    log(message, Error);
}

void Logger::success(const QString& message) {
    log(message, Success);
}

QString Logger::getLogBuffer() const {
    QMutexLocker locker(&m_mutex);
    return m_logBuffer.join("\n");
}

void Logger::clearLogBuffer() {
    QMutexLocker locker(&m_mutex);
    m_logBuffer.clear();
}