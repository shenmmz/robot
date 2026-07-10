/*
 * @file ServoController.h
 * @brief 舵机角度控制类（支持速度控制）
 *
 * 本文件定义 ServoController 类，该类负责：
 * - 角度到占空比的转换
 * - 提供用户友好的舵机角度控制接口
 * - 支持速度控制（平滑转动到目标角度）
 * - 实际的通信委托给Rs485Controller
 */
#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QList>
#include <QMap>

class Rs485Controller;

/**
 * @brief 单个舵机状态
 */
struct ServoState {
    float currentAngle = 90.0f;    // 当前角度
    float targetAngle = 90.0f;    // 目标角度
    float speed = 10.0f;          // 转动速度（度/秒）
    bool isMoving = false;         // 是否正在移动
};

/**
 * @brief 软插值计划项（单通道）
 *
 * 一次 setAngleWithSpeed / setAnglesForScript 调用，对每个长距离通道
 * 生成一份计划，由 100ms tick 推进。
 */
struct ChannelMovePlan {
    int   channel       = 0;        // 通道号 (0-5)
    float targetAngle   = 0.0f;     // 目标角度
    float stepPerTick   = 0.0f;     // 每 100ms tick 应推进的角度（带方向）
    int   totalTicks    = 0;        // 计划 tick 总数
    int   currentTick   = 0;        // 已执行 tick 数
};

/**
 * @brief 舵机控制器（支持速度控制）
 *
 * 只负责角度到占空比的转换，实际通信委托给Rs485Controller。
 * 提供用户友好的舵机角度控制接口（0-180度），支持速度控制。
 */
class ServoController : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param rs485 Rs485Controller指针
     * @param parent 父对象
     */
    explicit ServoController(Rs485Controller* rs485, QObject *parent = nullptr);
    ~ServoController();

    /**
     * @brief 设置RS485控制器
     * @param rs485 Rs485Controller指针
     */
    void setRs485Controller(Rs485Controller* rs485);

    /**
     * @brief 检查通道号是否合法
     * @param channel 通道号 (0-5)
     * @return 合法返回true
     */
    bool checkChannel(int channel) const;

    /**
     * @brief 获取当前角度
     * @param channel 通道号 (0-5)
     * @return 当前角度
     */
    float getCurrentAngle(int channel) const;

    /**
     * @brief 获取目标角度
     * @param channel 通道号 (0-5)
     * @return 目标角度
     */
    float getTargetAngle(int channel) const;

    /**
     * @brief 获取速度
     * @param channel 通道号 (0-5)
     * @return 速度（度/秒）
     */
    float getSpeed(int channel) const;

    /**
     * @brief 检查舵机是否正在移动
     * @param channel 通道号 (0-5)
     * @return 正在移动返回true
     */
    bool isMoving(int channel) const;

public slots:
    /**
     * @brief 设置舵机角度（脚本模式：阈值分流，速度参数生效）
     *
     * 行为（2026-06-15 二次优化）：
     *   - 短距离（|Δ| ≤ SHORT_MOVE_THRESHOLD_DEG）：保留 2026-06-15 单帧直发
     *     快速通道，物理到位由舵机自身响应（无延迟）
     *   - 长距离（|Δ| > SHORT_MOVE_THRESHOLD_DEG）：100ms tick 软插值，
     *     stepPerTick = speed × 0.1，依赖 Rs485Controller::m_cachedRegs diff
     *     缓存（命中后单路仅 14-28ms RPC ≪ 100ms tick → 平滑）
     *   - speed 范围限制 1-360°/s（用户选定范围）
     *   - 软插值期间 isMoving=true，TaskExecutor::waitForServoIdle 阻塞等待
     *
     * 设计动机：旧版 50ms tick 走 14 次同步 RPC（≈200ms 实际周期）造成
     * 0.5° 阶梯抖动；新版用 diff 缓存 + 100ms tick 让 RPC 永远小于 tick 周期。
     *
     * @param channel 通道号 (0-5)
     * @param angle 目标角度 (0-180度, 匹配KPZ32舵机范围)
     * @param speed 转动速度（度/秒），范围 1-360；超出截断
     * @return 发送成功返回true
     */
    bool setAngleWithSpeed(int channel, float angle, float speed = 10.0f);

    /**
     * @brief 脚本模式批量下发舵机角度（多路支持速度控制，单帧 6 路下发）
     *
     * 行为：
     *   - 对每个目标通道按 |Δ| 是否 > SHORT_MOVE_THRESHOLD_DEG 决定走
     *     单帧快速通道还是 100ms tick 软插值
     *   - 凡是走软插值的通道，**共享一个 100ms timer**，每 tick 用 setAllPwm
     *     同帧下发 6 路中间角度（diff 缓存命中后 ~30-50ms < 100ms tick）
     *   - 全是短距离 → 保留 2026-06-15 单帧 setAllPwm 行为，无 isMoving 等待
     *   - 至少一路长距离 → 启动共享 timer；isMoving=true；最后一个通道到位后
     *     各自 emit moveCompleted 并停止 timer
     *
     * @param channels  目标通道列表（0-5）
     * @param angles    目标角度列表（与 channels 一一对应）
     * @param speeds    速度列表（与 channels 一一对应；1-360°/s）
     * @return 全部成功返回 true；入参不一致或任一通道非法返回 false
     */
    bool setAnglesForScript(const QList<int>& channels,
                            const QList<float>& angles,
                            const QList<float>& speeds);

    /**
     * @brief 脚本模式批量下发舵机角度（支持"时间模式"与"速度模式"混合）
     *
     * 2026-06-16 新增：仿 KPZ32 出厂协议 `#xxxPxxxT<ms>!` 的语义，每路可独立选择
     * 速度（°/s）或时间（ms）控制。times[i] > 0 走时间模式，否则按 speeds[i] 走速度模式。
     *
     * 时间模式参数说明：
     *   - times[i] = 0   → 走速度分支，使用 speeds[i]（°/s）
     *   - times[i] < 20  → 单帧直发（瞬时到位，对齐 KPZ32 T<20 的行为）
     *   - times[i] >= 20 → 100ms tick 软插值，totalTicks = times[i] / SOFT_INTERP_TICK_MS
     *                       受 MAX_INTERP_TICKS(300 = 30s) 限制
     *   - times[i] > 30000 → 警告并强制 30000
     *
     * 与 setAnglesForScript 的差别仅在于：长距离通道构造 plan 时用 times[i] 而非
     * `Δ / speed` 反算。其它行为（短距离单帧、共享 timer、diff 缓存、信号 emit）一致。
     *
     * @param channels  目标通道列表（0-5）
     * @param angles    目标角度列表（与 channels 一一对应）
     * @param speeds    速度列表（与 channels 一一对应；1-360°/s；时间模式下忽略）
     * @param times     时间列表（与 channels 一一对应；单位 ms；0 走速度分支）
     * @return 全部成功返回 true；入参不一致或任一通道非法返回 false
     */
    bool setAnglesForScriptMixed(const QList<int>& channels,
                                 const QList<float>& angles,
                                 const QList<float>& speeds,
                                 const QList<int>& times);

    /**
     * @brief 拖拽模式设置角度（仅更新内部状态，不发送）
     *
     * 用于UI拖拽滑块场景。直接更新 currentAngle 和 targetAngle，
     * 标记 isMoving=true，由 onTimerTick 的批量帧统一发送。
     * 这样所有通道通过同一帧发送，P05/P06 不会滞后。
     *
     * @param channel 通道号 (0-5)
     * @param angle 目标角度 (0-180度)
     * @return 设置成功返回true
     */
    bool setAngleDrag(int channel, float angle);

    /**
     * @brief 设置默认速度
     * @param channel 通道号 (0-5)
     * @param speed 速度（度/秒）
     */
    void setDefaultSpeed(int channel, float speed);

    /**
     * @brief 停止指定通道的PWM输出
     * @param channel 通道号 (0-5)
     * @return 停止成功返回true
     */
    bool stop(int channel);

    /**
     * @brief 停止所有通道的PWM输出
     * @return 所有通道停止成功返回true
     */
    bool stopAll();

    /**
     * @brief 初始化所有通道（设置默认频率50Hz，角度90度）
     * @return 初始化成功返回true
     */
    bool initAll();

    /**
     * @brief 取消所有进行中的软插值（STOP/PAUSE 时调用）
     *
     * 停止所有活跃的 100ms 软插值 timer，将各通道 isMoving 复位为 false，
     * targetAngle 回退到 currentAngle（停留在当前位置），便于
     * TaskExecutor::waitForServoIdle 立即返回。
     *
     * 不会发送任何帧（用户可以随后手动重发 SERVO / 拖拽）。
     */
    void cancelAllMovements();

signals:
    /**
     * @brief 角度改变信号
     * @param channel 通道号
     * @param angle 当前角度
     */
    void angleChanged(int channel, float angle);

    /**
     * @brief 移动开始信号
     * @param channel 通道号
     */
    void moveStarted(int channel);

    /**
     * @brief 移动完成信号
     * @param channel 通道号
     * @param angle 最终角度
     */
    void moveCompleted(int channel, float angle);

    /**
     * @brief 错误信号
     * @param message 错误信息
     */
    void errorOccurred(const QString& message);

private slots:
    /**
     * @brief 定时器回调，逐步更新角度
     */
    void onTimerTick();

private:
    Rs485Controller* m_rs485;       // RS485控制器指针
    int m_channelCount;              // PWM通道数量
    QString m_lastError;            // 最后一次错误信息
    QTimer* m_timer;                // 速度控制定时器（UI 拖拽模式）
    QMap<int, ServoState> m_states;  // 各通道状态
    QList<QTimer*> m_activeTimers;    // 活跃的 100ms 软插值 timer（脚本模式），cancelAllMovements 时一并清理

    static const float MIN_ANGLE;     // 最小角度 (30°)
    static const float MAX_ANGLE;    // 最大角度 (140°)
    static const quint16 DEFAULT_FREQ_HZ; // 默认频率 (50Hz)
    static const quint16 CENTER_DUTY; // 中位占空比 (75/1000 = 7.5%)
    static const int TIMER_INTERVAL_MS; // UI 拖拽定时器间隔（毫秒）
    static const int SOFT_INTERP_TICK_MS; // 脚本模式软插值定时器间隔（毫秒，默认 100）
    static const float SHORT_MOVE_THRESHOLD_DEG; // 单帧直发阈值（角度），≤ 此值走快速通道
    static const float MIN_SCRIPT_SPEED;        // 脚本速度下限（度/秒）
    static const float MAX_SCRIPT_SPEED;        // 脚本速度上限（度/秒）
    static const int   MAX_INTERP_TICKS;        // 软插值 tick 上限（超过按此值截断）

    /**
     * @brief 角度转占空比（千分比）
     * @param angle 角度 (0-180度)
     * @return 占空比千分比 (25-125)  对应脉宽500us-2500us
     */
    static quint16 angleToDutyPermil(float angle);

    /**
     * @brief 单帧下发单通道目标角度（脚本模式私有辅助）
     *
     * 内部走 m_rs485->setServoAngle → sendRawModbusFrame；不做软插值，
     * 不更新内部 currentAngle/targetAngle，由调用方负责状态机推进。
     *
     * @param channel 通道号 (0-5)
     * @param angle   目标角度（调用前自行限幅）
     */
    void sendSingleFrame(int channel, float angle);

    /**
     * @brief 启动 100ms tick 软插值（共享 timer 多通道并发）
     *
     * 将传入的 plans 装入一个新 QTimer，每 100ms 推进一次：
     * - 每 tick 推进每个 channel.stepPerTick
     * - 每 tick 调用一次 m_rs485->setAllPwm() 同帧下发 6 路（diff 缓存）
     * - 所有 channel 走完后 emit moveCompleted 并 deleteLater timer
     *
     * 新 timer 自动加入 m_activeTimers 列表，便于 cancelAllMovements 清理。
     *
     * @param plans 软插值计划列表
     * @return 新创建的 QTimer 指针（失败返回 nullptr）
     */
    QTimer* startSoftInterpolation(const QList<ChannelMovePlan>& plans);

    /**
     * @brief 设置错误信息
     * @param msg 错误信息
     */
    void setError(const QString& msg);
};

#endif // SERVOCONTROLLER_H
