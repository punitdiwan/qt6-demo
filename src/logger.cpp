#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>

Logger &Logger::instance()
{
    static Logger inst;
    return inst;
}

Logger::Logger() {}

Logger::~Logger()
{
    if (m_file.isOpen())
        m_file.close();
}

void Logger::setLogFile(const QString &path)
{
    QMutexLocker lock(&m_mutex);
    if (m_file.isOpen())
        m_file.close();

    QDir().mkpath(QFileInfo(path).absolutePath());
    m_file.setFileName(path);

    if (m_file.open(QIODevice::Append | QIODevice::Text))
        m_stream.setDevice(&m_file);
}

QString Logger::logFilePath() const
{
    return m_file.fileName();
}

void Logger::write(const QString &category, const QString &message)
{
    auto &inst = instance();
    QMutexLocker lock(&inst.m_mutex);
    if (!inst.m_file.isOpen())
        return;

    inst.m_stream
        << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
        << " [" << category << "] "
        << message << "\n";
    inst.m_stream.flush();
}
