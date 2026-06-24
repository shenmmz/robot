/*
 * @file Rs485Controller.h
 * @brief RS485 Modbus通信控制器，通过ZMC控制器的Modbus转RS485功能实现。
 *
 * 本文件定义 Rs485Controller 类，该类负责：
 * - 通过ZMC控制器发送Modbus RTU命令到外部RS485设备
 * - 控制外部6路PWM模块（QD-6PWM-V2.0）
 * - 提供舵机角度控制接口
 */
#ifndef RS485CONTROLLER_H
#define RS485CONTROLLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>

// 前向声明ZmotionAPI
class ZmotionAPI;

/**
 * @brief RS485 Modbus通信控制器
 *
 * 通过ZMC控制器的Modbus转RS485功能与外部设备通信。
 * 发送Modbus RTU命令到RS485总线上的从机设备。
 * 
 * 线程安全说明：
 *   - 本类的所有公共方法都是同步阻塞的
 *   - 建议在工作线程中调用,避免阻塞UI线程
 *   - 或者使用Qt的信号槽机制异步调用
 *   - 类内部不使用QThread::msleep等阻塞调用
 */
class Rs485Controller : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit Rs485Controller(QObject *parent = nullptr);
    ~Rs485Controller();

    /**
     * @brief 设置ZMotion API指针
     * @param api ZmotionAPI指针
     */
    void setZmotionAPI(ZmotionAPI* api);

    /**
     * @brief 获取ZMotion API指针
     * @return ZmotionAPI指针
     */
    ZmotionAPI* getZmotionAPI() const { return m_zmotionAPI; }

    /**
     * @brief 获取当前通信状态
     * @return 已连接返回true
     */
    bool isConnected() const { return m_zmotionAPI != nullptr; }

signals:
    // 通信成功
    void communicationSuccess();

    // 通信失败
    void communicationFailed(const QString& errorMessage);

    // 发送完成
    void dataSent(qint64 bytesWritten);

    // 错误发生
    void errorOccurred(const QString& errorMessage);

public:
    // ============ PWM控制接口 ============

    /**
     * @brief 设置单个PWM输出的频率和占空比（单路PWM协议）
     * 
     * 数据格式：频率(3B) + 占空比(2B) + 填充(1B) = 6字节
     * 寄存器地址：PWM1=0x2251, PWM2=0x2252, ..., PWM6=0x2256
     * 
     * @param slaveAddress 从站地址 (如 0x21)
     * @param pwmIndex PWM通道索引 (1-6)
     * @param frequency 频率 (Hz，范围 1-100000)
     * @param dutyCycle 占空比千分比 (0-1000，表示 0.0%-100.0%)
     * @return 发送成功返回true
     */
    bool setSinglePwmFrequencyAndDuty(quint8 slaveAddress, quint8 pwmIndex, quint32 frequency, quint16 dutyCycle);

    /**
     * @brief 设置单个PWM占空比（使用默认地址和频率）
     * @param pwmIndex PWM通道索引 (1-6)
     * @param dutyCycle 占空比千分比 (0-1000)
     * @return 发送成功返回true
     */
    bool setPwmDuty(quint8 pwmIndex, quint16 dutyCycle);

    /**
     * @brief 设置舵机角度（0-180度，自动转换为占空比，匹配KPZ32舵机范围）
     * @param pwmIndex PWM通道索引 (1-6)
     * @param angle 舵机角度 (0-180度)
     * @return 发送成功返回true
     */
    bool setServoAngle(quint8 pwmIndex, float angle);

    /**
     * @brief 设置所有6路PWM（批量设置）
     * @param slaveAddress 从站地址
     * @param freq1-freq4 PWM1-PWM4频率 (Hz)
     * @param duty1-duty6 占空比千分比 (0-1000)
     * @return 发送成功返回true
     */
    bool setAllPwm(quint8 slaveAddress,
                   quint32 freq1, quint16 duty1,
                   quint32 freq2, quint16 duty2,
                   quint32 freq3, quint16 duty3,
                   quint32 freq4, quint16 duty4,
                   quint16 duty5, quint16 duty6);

    /**
     * @brief 通过ZMC发送Modbus请求（ZMC内部自动组帧，含CRC/帧头）
     *
     * ZMC控制器负责组装完整的Modbus RTU帧并自动计算CRC，
     * 用户只需提供从站地址、功能码、起始地址和原始数据即可。
     *
     * @param slaveAddress 从站地址
     * @param functionCode 功能码
     * @param startAddress 起始地址
     * @param data 数据部分（不包含CRC）
     * @return 发送成功返回true
     */
    bool sendModbusRequest(quint8 slaveAddress, quint8 functionCode, quint16 startAddress, const QByteArray& data);

    /**
     * @brief 通过RAW模式+PUTCHAR发送完整Modbus RTU帧（用户自行组帧）
     *
     * 用户需要自行拼装完整的Modbus帧结构（地址+功能码+寄存器数+数据），
     * CRC由本函数自动计算追加。底层切换到RAW透传模式，逐字节通过PUTCHAR发送，
     * 发送完毕后自动恢复Modbus主端模式。
     *
     * @param slaveAddress 从站地址
     * @param functionCode 功能码
     * @param startAddress 起始寄存器地址
     * @param data 数据字节（不包含CRC，会自动追加）
     * @return 发送成功返回true
     */
    bool sendRawModbusFrame(quint8 slaveAddress, quint8 functionCode,
                            quint16 startAddress, const QByteArray& data);

private:
    ZmotionAPI* m_zmotionAPI = nullptr;  // ZMotion API指针
    quint8 m_slaveAddress = 0x21;        // 默认从站地址
    quint32 m_defaultFrequency = 50;     // 默认频率50Hz
    quint32 m_pwm1Frequency = 50;        // PWM1频率（PWM5/PWM6与其相同）
    int m_localRegBase = 0;              // MODBUS_REG本地数据起始索引（发送从此开始，读取从base+wordCount开始）
    bool m_serialConfigured = false;     // SETCOM是否已配置（避免每次请求重复配置）

    // ========== 发送缓存（2026-06-15 优化：避免每帧 14 次同步 RPC）==========
    quint8 m_cachedSlaveAddress = 0xFF;   // 0xFF = 未缓存；ZMC 会记忆从站，重复设置是冗余
    QVector<quint16> m_cachedRegs;       // 上次写入的 12 个 MODBUS_REG 镜像；用于 diff 写入

    /**
     * @brief 计算Modbus CRC16校验码
     * @param data 数据字节数组
     * @return CRC16校验码
     */
    static quint16 calcModbusCRC16(const QByteArray& data);

    /**
     * @brief 设置错误信息
     * @param msg 错误信息
     */
    void setError(const QString& msg);
};

#endif // RS485CONTROLLER_H
