#pragma once

#include <QFile>
#include <QMutex>
#include <QString>
#include <QTextStream>

class Logger {
public:
    static Logger &instance();

    void    setLogFile(const QString &path);
    QString logFilePath() const;

    // Logger::write("Updater", "Checking for updates...")
    static void write(const QString &category, const QString &message);

private:
    Logger();
    ~Logger();

    QFile       m_file;
    QTextStream m_stream;
    QMutex      m_mutex;
};
