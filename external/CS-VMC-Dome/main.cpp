#include "mainwindow.h"

#include <QApplication>

/**
 * @brief Qt 应用程序入口。
 * @param argc 命令行参数数量。
 * @param argv 命令行参数数组。
 * @return Qt 事件循环退出码。
 *
 * 输入数据：系统传入的命令行参数。
 * 输出数据：创建并显示 MainWindow，进入 Qt 事件循环。
 */
int main(int argc, char *argv[])
{
    // 创建 Qt Widgets 应用对象，负责事件循环和全局应用状态。
    QApplication a(argc, argv);

    // 创建示例主窗口，主窗口内部会创建 VMControl 模块实例。
    MainWindow w;

    // 显示主窗口。
    w.show();

    // 启动 Qt 事件循环并返回退出码。
    return a.exec();
}
