/*
 * @file Logger.h
 * @brief 声明简单日志系统，用于记录应用运行日志。
 *
 * 本文件定义 Logger 类，支持：
 * - 信息日志
 * - 警告日志
 * - 错误日志
 * - 日志文件写入
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>

/**
 * @brief 简单日志系统
 * 
 * 使用说明：
 * Logger::info("应用启动");
 * Logger::warning("配置文件未找到");
 * Logger::error("硬件连接失败");
 */
class Logger
{
public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    // 初始化日志文件
    static void initialize(const QString& logFilePath);

    // 日志输出
    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);

private:
    static LogLevel s_minLevel;
    static QFile* s_logFile;
    static QTextStream* s_stream;

    static void log(LogLevel level, const QString& message);
    static QString levelToString(LogLevel level);
};

#endif // LOGGER_H
