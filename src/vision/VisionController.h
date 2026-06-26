/*
 * @file VisionController.h
 * @brief 启动并管理外部 VisualRoboticArmQtController.exe 子进程。
 *
 * 职责：
 * - 解析外部 exe 路径（优先 Release 配置，回退 Debug）
 * - 用 QProcess 启动外部 Qt 主控程序（该程序自身包含 TCP Server + Python 视觉子进程 + UI）
 * - 监控子进程的 stdout / stderr / 退出事件
 * - 提供 isRunning() / stop() 业务接口和 started / stopped / errorOccurred / outputMessage 信号
 *
 * 不在此类中实现的功能：
 * - 与外部程序的进程间通讯（外部程序已自带 UI，所有姿态数据在其窗口内展示）
 * - 路径配置持久化（路径目前硬编码，未来应迁移到 robot_config.json）
 */
#ifndef VISIONCONTROLLER_H
#define VISIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>

class QProcess;

/**
 * @brief 功能测试：外部视觉程序子进程控制器
 *
 * 用法示例（MainWindow 中）：
 * @code
 *   m_vision = new VisionController(this);
 *   connect(m_vision, &VisionController::started,        this, &MainWindow::onVisionStarted);
 *   connect(m_vision, &VisionController::stopped,        this, &MainWindow::onVisionStopped);
 *   connect(m_vision, &VisionController::errorOccurred,  this, &MainWindow::onVisionError);
 *   connect(m_vision, &VisionController::outputMessage, this, &MainWindow::onVisionMessage);
 *
 *   if (m_vision->isRunning()) {
 *       m_vision->stop();
 *   } else {
 *       m_vision->start();   // 内部自动解析默认 exe 路径
 *   }
 * @endcode
 */
class VisionController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent Qt 父对象（通常为 MainWindow）
     */
    explicit VisionController(QObject *parent = nullptr);

    ~VisionController() override;

    /**
     * @brief 启动外部 VisualRoboticArmQtController.exe
     *
     * 路径解析顺序：
     *  1. 若调用过 setExePath()，使用设置的路径
     *  2. 否则尝试默认 Release 路径
     *  3. 否则尝试默认 Debug 路径
     *  4. 都失败则通过 errorOccurred 信号报告
     *
     * @return true 表示 QProcess.start() 成功；false 表示启动失败
     */
    bool start();

    /**
     * @brief 主动停止外部子进程
     *
     * 先尝试 terminate()（让外部程序走 closeEvent 优雅退出），
     * 若 1500ms 内未退出则 kill() 强制结束。
     */
    void stop();

    /**
     * @brief 查询当前子进程是否在运行
     */
    bool isRunning() const;

    /**
     * @brief 获取当前使用的外部 exe 路径
     * @return 若未设置或未启动过，返回空字符串
     */
    QString exePath() const;

    /**
     * @brief 设置外部 exe 路径（覆盖默认路径解析）
     * @param path exe 绝对路径
     */
    void setExePath(const QString &path);

signals:
    /**
     * @brief 子进程启动成功（QProcess::started 转发）
     */
    void started();

    /**
     * @brief 子进程已停止（无论是主动 stop 还是异常退出）
     * @param exitCode 子进程退出码
     */
    void stopped(int exitCode);

    /**
     * @brief 子进程错误（启动失败、崩溃、读写错误等）
     * @param error 错误描述字符串
     */
    void errorOccurred(const QString &error);

    /**
     * @brief 子进程 stdout/stderr 输出一行（用于诊断显示）
     * @param channel 通道名（"stdout" / "stderr"）
     * @param line 文本内容
     */
    void outputMessage(const QString &channel, const QString &line);

private slots:
    void onStarted();
    void onReadyReadStdout();
    void onReadyReadStderr();
    void onFinished(int exitCode, int exitStatus);
    void onProcessError(int error);

private:
    /**
     * @brief 解析默认的外部 exe 路径（Release 优先，回退 Debug）
     * @return 第一个存在的路径；若都不存在返回空字符串
     */
    static QString resolveDefaultExePath();

    /**
     * @brief 从 QByteArray 中按行切分并 emit outputMessage
     */
    void drainLines(const QByteArray &data, const QString &channel);

    QProcess *m_process = nullptr;
    QString m_exePath;           // 用户显式设置的 exe 路径（空表示自动解析）
    QString m_resolvedExePath;   // 实际启动时使用的路径
    QByteArray m_stdoutBuffer;   // 处理半行/多行情况的行缓冲
    QByteArray m_stderrBuffer;
};

#endif // VISIONCONTROLLER_H