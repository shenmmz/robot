/*
 * @file VisionController.h
 * @brief 启动 Python 视觉程序并以 TCP Server 接收姿态数据。
 *
 * 职责：
 * - 用 QProcess 启动 Python 视觉程序（python -m main，默认连接 5001）
 * - 开启 QTcpServer 监听 5001，接收 Python 的 pose 数据
 * - 解析 JSON 提取 motor_6.angle_deg，通过 motorCommand 信号下发
 * - 监控子进程的 stdout / stderr / 退出事件
 * - 提供 isRunning() / stop() 业务接口和 started / stopped / errorOccurred / outputMessage 信号
 *
 * 不再依赖外部 VisualRoboticArmQtController.exe，绕过其单连接限制。
 */
#ifndef VISIONCONTROLLER_H
#define VISIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>

class QProcess;
class QTcpServer;
class QTcpSocket;

class VisionController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造 VisionController,创建并初始化内部的 QTcpServer。
     *
     * 负责:
     * - 创建 QTcpServer 实例并将其父子对象设为 this,保证生命周期随本对象释放。
     * - 将 QTcpServer::newConnection 信号连接至本对象的 onNewConnection 槽。
     *
     * @param parent Qt 父对象指针,默认 nullptr。
     */
    explicit VisionController(QObject *parent = nullptr);

    /**
     * @brief 析构函数,负责安全释放子进程与 TCP Server 资源。
     *
     * 释放顺序:
     * - 若 Python 子进程仍在运行,先 terminate(),等待 1 秒后若仍未退出则 kill() 强杀。
     * - 关闭 QTcpServer,断开所有 TCP 连接。
     */
    ~VisionController() override;

    /**
     * @brief 启动 TCP Server 并拉起 Python 视觉程序
     * @return true 表示 Server 监听成功且 QProcess 启动成功
     */
    bool start();

    /**
     * @brief 停止 Python 子进程并关闭 TCP Server
     */
    void stop();

    /**
     * @brief 判断 Python 视觉子进程是否仍在运行。
     * @return true 表示 m_process 非空且状态非 NotRunning。
     */
    bool isRunning() const;

    /**
     * @brief 获取 Python 项目目录路径
     */
    QString projectPath() const;

signals:
    /**
     * @brief Python 视觉程序已成功启动时触发(QProcess::started 之后)。
     */
    void started();

    /**
     * @brief Python 视觉程序退出时触发。
     * @param exitCode Python 进程的退出码。
     */
    void stopped(int exitCode);

    /**
     * @brief 启动失败或子进程发生 errorOccurred 时触发。
     * @param error 人类可读的错误描述。
     */
    void errorOccurred(const QString &error);

    /**
     * @brief 子进程 stdout/stderr 产生一行输出时触发(已按行切分)。
     * @param channel "stdout" 或 "stderr"。
     * @param line    去除末尾换行符的整行文本。
     */
    void outputMessage(const QString &channel, const QString &line);

    /**
     * @brief 视觉系统下发 J0 旋转角度指令
     * @param angle 目标角度（度）
     */
    void motorCommand(double angle);

private slots:
    /**
     * @brief QProcess::started 信号槽,触发后立即对外 emit started()。
     */
    void onStarted();

    /**
     * @brief QProcess::readyReadStandardOutput 信号槽。
     *
     * 读取子进程 stdout 的全部可用数据,交给 drainLines 按行切分并 emit outputMessage。
     */
    void onReadyReadStdout();

    /**
     * @brief QProcess::readyReadStandardError 信号槽。
     *
     * 读取子进程 stderr 的全部可用数据,交给 drainLines 按行切分并 emit outputMessage。
     */
    void onReadyReadStderr();

    /**
     * @brief QProcess::finished 信号槽,Python 子进程退出时触发。
     *
     * 流程:
     * - 先把 stdout/stderr 缓冲区中未完成的一行冲刷出去。
     * - 记录 exitCode,qDebug 输出日志,emit stopped(exitCode)。
     * - 通过 deleteLater 释放 m_process 并置空指针。
     *
     * @param exitCode   Python 进程退出码。
     * @param exitStatus 退出状态(正常退出 / 崩溃),本函数暂不区分。
     */
    void onFinished(int exitCode, int exitStatus);

    /**
     * @brief QProcess::errorOccurred 信号槽。
     *
     * 仅把 QProcess::errorString 透传给 errorOccurred 信号,qWarning 记录日志。
     *
     * @param error QProcess 错误枚举值,本函数未使用。
     */
    void onProcessError(int error);

    // --- TCP Server ---

    /**
     * @brief QTcpServer::newConnection 信号槽。
     *
     * 单连接策略: 已存在 m_clientSocket 时,拒绝新连接并 deleteLater 立即释放;
     * 否则接受 nextPendingConnection 作为 m_clientSocket,绑定 readyRead / disconnected 信号。
     */
    void onNewConnection();

    /**
     * @brief QTcpSocket::readyRead 信号槽。
     *
     * 把当前可读数据追加进 m_tcpBuffer,按 '\n' 切包;每得到完整一行就调用 parseAndDispatch。
     */
    void onClientReadyRead();

    /**
     * @brief QTcpSocket::disconnected 信号槽。
     *
     * 记录日志,通过 deleteLater 释放 m_clientSocket 并置空指针。
     */
    void onClientDisconnected();

private:
    /**
     * @brief 将 QByteArray 数据按 '\n' 切行,并逐行 emit outputMessage。
     *
     * 根据 channel 选择累加到 m_stdoutBuffer 或 m_stderrBuffer;对未以 '\n' 结尾的残余字节保留在缓冲区,
     * 等待下次数据到达时继续拼接。
     *
     * @param data    本次 readyRead 读到的字节片段。
     * @param channel "stdout" 或 "stderr",决定使用哪个累加缓冲。
     */
    void drainLines(const QByteArray &data, const QString &channel);

    /**
     * @brief 解析一帧 JSON 并下发对应的业务信号。
     *
     * 协议约定(只关心 type=="pose" 的消息):
     * @code
     * {
     *   "type": "pose",
     *   "payload": {
     *     "motor_6": { "angle_deg": <double> }
     *   }
     * }
     * @endcode
     * 解析成功后 emit motorCommand(angle);任何环节缺失或 type 不匹配都会静默忽略。
     *
     * @param line 单行 UTF-8 JSON 文本(已去除 '\n')。
     */
    void parseAndDispatch(const QByteArray &line);

    QProcess *m_process = nullptr;
    QString m_projectPath;
    QByteArray m_stdoutBuffer;
    QByteArray m_stderrBuffer;

    QTcpServer *m_tcpServer = nullptr;
    QTcpSocket *m_clientSocket = nullptr;
    QByteArray m_tcpBuffer;
    static constexpr quint16 kServerPort = 5001;
};

#endif // VISIONCONTROLLER_H