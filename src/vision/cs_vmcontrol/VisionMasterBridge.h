#ifndef VISIONMASTERBRIDGE_H
#define VISIONMASTERBRIDGE_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QByteArray>

/**
 * @brief 视觉模块桥接层 — 通过 QProcess 启动 CS-VMC-Dome.exe。
 *
 * Robot_R06 不再内嵌 VisionMaster SDK，而是启动独立的 CS-VMC-Dome.exe 进程。
 * 用户在 CS-VMC-Dome 窗口中直接操作（加载方案、执行流程、查看结果）。
 *
 * 职责：
 *   - 启动 / 关闭 CS-VMC-Dome.exe
 *   - 监控进程状态
 *   - 向 UI 发送日志消息
 */
class VisionMasterBridge : public QObject
{
    Q_OBJECT

public:
    explicit VisionMasterBridge(QObject *parent = nullptr);
    ~VisionMasterBridge() override;

    /**
     * @brief 启动 CS-VMC-Dome.exe。
     * @return true 表示启动成功（进程已开始）。
     */
    bool launch();

    /**
     * @brief 关闭 CS-VMC-Dome.exe。
     */
    void terminate();

    /**
     * @brief CS-VMC-Dome.exe 是否正在运行。
     */
    bool isRunning() const;

signals:
    /// CS-VMC-Dome.exe 启动成功
    void processStarted();
    /// CS-VMC-Dome.exe 退出
    void processFinished(int exitCode);
    /// 日志消息（供 UI 显示）
    void logMessage(const QString &message);
    /// 视觉数据（从 CS-VMC-Dome stdout 捕获的解析行）
    void visionDataReceived(const QString &line);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void onReadyReadStandardOutput();

private:
    QProcess *m_process = nullptr;
    QString m_exePath;  ///< CS-VMC-Dome.exe 路径
    QByteArray m_stdoutBuffer;  ///< stdout 半行缓冲

    /** @brief 查找 CS-VMC-Dome.exe 路径 */
    QString findExePath() const;
};

#endif // VISIONMASTERBRIDGE_H
