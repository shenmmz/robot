/*
 * @file Logger.cpp
 * @brief 实现 Logger 类，提供简单的日志记录功能。
 *
 * 本文件负责：
 * - 日志等级处理
 * - 控制台和文件输出
 * - 日志格式化
 */
#include "Logger.h"
#include <QDateTime>
#include <iostream>

Logger::LogLevel Logger::s_minLevel = Logger::INFO;
QFile* Logger::s_logFile = nullptr;
QTextStream* Logger::s_stream = nullptr;

/**
 * @brief 初始化日志系统
 *
 * 打开日志文件并创建输出流。日志以追加模式写入文件。
 *
 * @param logFilePath 日志文件路径
 */
void Logger::initialize(const QString& logFilePath)
{
    s_logFile = new QFile(logFilePath);
    
    if (s_logFile->open(QIODevice::Append | QIODevice::Text)) {
        s_stream = new QTextStream(s_logFile);
    }
}

/**
 * @brief 记录DEBUG级别日志
 *
 * @param message 日志消息
 */
void Logger::debug(const QString& message)
{
    log(DEBUG, message);
}

/**
 * @brief 记录INFO级别日志
 *
 * @param message 日志消息
 */
void Logger::info(const QString& message)
{
    log(INFO, message);
}

/**
 * @brief 记录WARNING级别日志
 *
 * @param message 日志消息
 */
void Logger::warning(const QString& message)
{
    log(WARNING, message);
}

/**
 * @brief 记录ERROR级别日志
 *
 * @param message 日志消息
 */
void Logger::error(const QString& message)
{
    log(ERROR, message);
}

/**
 * @brief 通用日志记录
 *
 * 根据日志等级判断是否输出，格式化日志消息后输出到控制台和文件。
 *
 * @param level 日志等级
 * @param message 日志消息
 */
void Logger::log(LogLevel level, const QString& message)
{
    if (level < s_minLevel) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString logMessage = QString("[%1] [%2] %3")
                         .arg(timestamp)
                         .arg(levelToString(level))
                         .arg(message);

    // 输出到控制台
    std::cout << logMessage.toStdString() << std::endl;

    // 输出到文件
    if (s_stream) {
        *s_stream << logMessage << "\n";
        s_stream->flush();
    }
}

/**
 * @brief 日志等级转字符串
 *
 * @param level 日志等级
 * @return QString 等级对应的字符串："DEBUG"、"INFO"、"WARN"、"ERROR"或"UNKNOWN"
 */
QString Logger::levelToString(LogLevel level)
{
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
