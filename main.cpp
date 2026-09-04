/*
 * @file main.cpp
 * @brief 程序入口，初始化 Qt 应用并显示主窗口。
 *
 * 本文件负责：
 * - 创建 QApplication 实例
 * - 创建并显示主窗口
 * - 启动 Qt 事件循环
 */
#include "mainwindow.h"
#include "src/utils/OsKEventFilter.h"   // 全局系统屏幕键盘事件过滤器

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <Windows.h>
#include <cstdio>

// ===== 文件日志处理器 =====
// 解决 VisionMaster SDK 初始化后 qDebug 输出被 SDK 内部日志缓冲的问题。
// 将所有 qDebug / qWarning / qCritical 输出实时写入日志文件 + stderr，
// 每条消息写入后立即 flush，确保日志文件始终是最新的（不受 SDK 缓冲影响）。

static QFile*  g_logFile = nullptr;   // 全局日志文件
static QMutex  g_logMutex;            // 线程安全互斥锁（qDebug 可从任意线程调用）

void fileMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&g_logMutex);

    // 时间戳（精确到毫秒）
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    // 级别标签（单字符，减少日志长度）
    QString level;
    switch (type) {
    case QtDebugMsg:    level = "D"; break;
    case QtInfoMsg:     level = "I"; break;
    case QtWarningMsg:  level = "W"; break;
    case QtCriticalMsg: level = "C"; break;
    case QtFatalMsg:    level = "F"; break;
    }

    // 只取文件名（不要完整路径）
    QString fileName = QFileInfo(context.file).fileName();

    // 构建日志行
    QString line;
    if (!fileName.isEmpty() && context.line > 0) {
        line = QString("[%1 %2] %3 (%4:%5)")
               .arg(timestamp, level, msg, fileName).arg(context.line);
    } else {
        line = QString("[%1 %2] %3").arg(timestamp, level, msg);
    }

    // 写入文件（UTF-8 编码，立即 flush，不受 SDK 缓冲影响）
    if (g_logFile && g_logFile->isOpen()) {
        g_logFile->write(line.toUtf8());
        g_logFile->write("\n");
        g_logFile->flush();
    }

    // 输出到控制台（本地编码 GBK，匹配 Windows 控制台 / Qt Creator 输出面板）
    QByteArray localLine = line.toLocal8Bit();
    fprintf(stderr, "%s\n", localLine.constData());
    fflush(stderr);

    if (type == QtFatalMsg) {
        abort();
    }
}

int main(int argc, char *argv[])
{
    // qputenv("QT_DEBUG_PLUGINS", "1");//调试使用
    QApplication a(argc, argv);

    // === 全局放大 QMessageBox（确认/提示弹窗），适配触屏操作 ===
    // 统一放大：文字 20px、按钮 18px 且加宽加高（便于手指点按）、窗口最小宽度 560。
    // 作用于所有 QMessageBox（回零确认、原点写入提示、错误/警告弹窗等）。
    a.setStyleSheet(QStringLiteral(
        "QMessageBox{min-width:560px;}"
        "QMessageBox QLabel{font-size:20px;color:#1a1a2e;}"
        "QMessageBox QPushButton{font-size:18px;min-width:140px;min-height:52px;"
        "padding:6px 18px;border-radius:6px;}"));

    // === 初始化文件日志 ===
    // 将所有 qDebug 输出实时写入文件，不受 VisionMaster SDK 缓冲影响。
    // 日志文件位于 exe 目录的 log/app/ 下，按启动时间命名。
    {
        QString logDir = a.applicationDirPath() + "/log/app";
        QDir().mkpath(logDir);
        QString logFileName = logDir + "/" +
            QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log";
        g_logFile = new QFile(logFileName);
        if (g_logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
            // 写入 UTF-8 BOM，确保 Windows 记事本能正确识别编码
            g_logFile->write("\xEF\xBB\xBF");
            g_logFile->write(QString("=== Robot_R06 Application Log ===\n"
                                     "Started: %1\n"
                                     "Qt Version: %2\n"
                                     "==================================\n")
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                            .arg(QT_VERSION_STR).toUtf8());
            g_logFile->flush();
        }
        qInstallMessageHandler(fileMessageHandler);
        qDebug() << "=== File logging started ===";
        qDebug() << "Log file:" << logFileName;
    }

    // 安装全局系统屏幕键盘(TabTip/osk.exe)事件过滤器:
    // 装到 qApp 后,所有 QAbstractSpinBox / QLineEdit 获得焦点都会弹系统键盘
    auto* oskFilter = new OsKEventFilter(qApp);  // parent 跟应用,无需手动 delete
    qApp->installEventFilter(oskFilter);
    MainWindow w;
    w.show();
    int ret = a.exec();

    // 清理日志文件
    if (g_logFile) {
        qDebug() << "=== Application shutting down, exit code:" << ret << "===";
        g_logFile->close();
        delete g_logFile;
        g_logFile = nullptr;
    }
    return ret;
}
