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

#include <QApplication>
#include <QDebug>
#include <Windows.h>
#include <iostream>

// // 自定义Qt消息处理器，将调试信息输出到控制台（暂时关闭调试窗口）
// void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
// {
//     QByteArray localMsg = msg.toLocal8Bit();
//     const char *file = context.file ? context.file : "";
//     const char *function = context.function ? context.function : "";
//     switch (type) {
//     case QtDebugMsg:
//         fprintf(stderr, "[Debug] %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
//         break;
//     case QtInfoMsg:
//         fprintf(stderr, "[Info] %s\n", localMsg.constData());
//         break;
//     case QtWarningMsg:
//         fprintf(stderr, "[Warning] %s\n", localMsg.constData());
//         break;
//     case QtCriticalMsg:
//         fprintf(stderr, "[Critical] %s\n", localMsg.constData());
//         break;
//     case QtFatalMsg:
//         fprintf(stderr, "[Fatal] %s\n", localMsg.constData());
//         break;
//     }
// }

int main(int argc, char *argv[])
{
    // // 暂时关闭调试窗口
    // AllocConsole();  // 打开调试控制台窗口
    // freopen("CONOUT$", "w", stdout);  // 重定向标准输出到控制台
    // freopen("CONOUT$", "w", stderr);  // 重定向错误输出到控制台（qDebug输出到stderr）

    // qInstallMessageHandler(customMessageHandler);  // 安装自定义消息处理器

    // qputenv("QT_DEBUG_PLUGINS", "1");//调试使用
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
