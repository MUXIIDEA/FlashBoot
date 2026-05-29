#ifndef LOGGER_H
#define LOGGER_H

#include "pch.h"

class Logger : public QObject {
    Q_OBJECT

public:
    enum LogLevel {
        Info = 0,
        Warning = 1,
        Error = 2,
        Success = 3
    };

    static Logger* instance();

    void log(const QString& message, LogLevel level = Info);
    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void success(const QString& message);

    QString getLogBuffer() const;
    void clearLogBuffer();

signals:
    void newLog(const QString& message, LogLevel level);

private:
    Logger(QObject* parent = nullptr);
    ~Logger();

    static Logger* m_instance;
    QStringList m_logBuffer;
    mutable QMutex m_mutex;
};

#endif // LOGGER_H