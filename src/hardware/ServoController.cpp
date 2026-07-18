/*
 * @file ServoController.cpp
 * @brief 舵机角度控制类实现（支持速度控制）
 *
 * 通过定时器逐步增量控制舵机转动速度。
 */
#include "ServoController.h"
#include "Rs485Controller.h"
#include <QDebug>

// 静态常量定义
const float ServoController::MIN_ANGLE = 0.0f;     // 0° 最小安全角度
const float ServoController::MAX_ANGLE = 180.0f;   // 180° 最大安全角度
const quint16 ServoController::DEFAULT_FREQ_HZ = 50;
const quint16 ServoController::CENTER_DUTY = 75;    // 7.5% = 90度位置 (1500us)
const int ServoController::TIMER_INTERVAL_MS = 50;   // UI 拖拽定时器间隔（毫秒）
const int   ServoController::SOFT_INTERP_TICK_MS  = 100;  // 脚本软插值 tick 间隔（毫秒）
const float ServoController::SHORT_MOVE_THRESHOLD_DEG = 30.0f;  // ≤ 30° 单帧直发
const float ServoController::MIN_SCRIPT_SPEED  = 1.0f;    // 脚本速度下限
const float ServoController::MAX_SCRIPT_SPEED  = 360.0f;  // 脚本速度上限（用户选定）
const int   ServoController::MAX_INTERP_TICKS  = 300;     // 软插值 tick 上限 = 30 秒（2026-06-16 放宽，原为 100）

/**
 * @brief 构造函数，初始化舵机控制器
 * @param rs485 Rs485Controller指针
 * @param parent 父对象
 */
ServoController::ServoController(Rs485Controller* rs485, QObject *parent)
    : QObject(parent)
    , m_rs485(rs485)
    , m_channelCount(6)
    , m_timer(new QTimer(this))
{
    // 初始化各通道状态
    for (int i = 0; i < m_channelCount; ++i) {
        ServoState state;
        state.currentAngle = 90.0f;
        state.targetAngle = 90.0f;
        state.speed = 1.0f; // 默认1度/秒（用户偏好：更慢，便于手动观察）
        state.isMoving = false;
        m_states[i] = state;
    }

    // 连接定时器
    connect(m_timer, &QTimer::timeout, this, &ServoController::onTimerTick);
    m_timer->start(TIMER_INTERVAL_MS); // 50ms周期
}

/**
 * @brief 析构函数
 */
ServoController::~ServoController()
{
    m_timer->stop();
}

/**
 * @brief 设置RS485控制器
 * @param rs485 Rs485Controller指针
 */
void ServoController::setRs485Controller(Rs485Controller* rs485)
{
    m_rs485 = rs485;
}

/**
 * @brief 检查通道号是否合法
 * @param channel 通道号 (0-5)
 * @return 合法返回true，否则返回false并记录错误信息
 */
bool ServoController::checkChannel(int channel) const
{
    if (!m_rs485) {
        const_cast<ServoController*>(this)->m_lastError = "RS485控制器未设置";
        return false;
    }
    if (channel < 0 || channel >= m_channelCount) {
        const_cast<ServoController*>(this)->m_lastError =
            QString("通道号无效: %1 (范围 0~%2)").arg(channel).arg(m_channelCount - 1);
        return false;
    }
    return true;
}

/**
 * @brief 获取当前角度
 */
float ServoController::getCurrentAngle(int channel) const
{
    if (!checkChannel(channel)) return 90.0f;
    return m_states[channel].currentAngle;
}

/**
 * @brief 获取目标角度
 */
float ServoController::getTargetAngle(int channel) const
{
    if (!checkChannel(channel)) return 90.0f;
    return m_states[channel].targetAngle;
}

/**
 * @brief 获取速度
 */
float ServoController::getSpeed(int channel) const
{
    if (!checkChannel(channel)) return 30.0f;
    return m_states[channel].speed;
}

/**
 * @brief 检查是否正在移动
 */
bool ServoController::isMoving(int channel) const
{
    if (!checkChannel(channel)) return false;
    return m_states[channel].isMoving;
}

/**
 * @brief 设置错误信息并输出警告日志
 */
void ServoController::setError(const QString &msg)
{
    m_lastError = msg;
    qWarning() << "[ServoController]" << msg;
    emit errorOccurred(msg);
}

/**
 * @brief 角度转占空比（千分比）
 *
 * KPZ32舵机 PWM 角度与占空比映射（50Hz, 周期20ms）：
 * - 0度   -> 500us -> 2.5%  = 25/1000
 * - 90度  -> 1500us -> 7.5% = 75/1000
 * - 180度 -> 2500us -> 12.5% = 125/1000
 *
 * 公式: dutyPermil = 25 + (angle / 180) * 100  (线性映射500us~2500us)
 *
 * @param angle 目标角度 (0-180度)
 * @return 占空比千分比 (25-125)
 */
quint16 ServoController::angleToDutyPermil(float angle)
{
    // 角度限幅
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    // KPZ32舵机: 占空比映射 500us(25) ~ 2500us(125)
    // 线性公式: dutyPermil = 25 + angle * (100 / 180)
    float dutyPermil = 25.0f + (angle / 180.0f) * 100.0f;

    return static_cast<quint16>(dutyPermil);
}

/**
 * @brief 设置舵机角度（带速度控制，平滑转动）
 *
 * 通过定时器控制实现舵机平滑转动到目标角度，支持独立设置各通道速度。
 * 当目标角度与当前位置不同时，启用定时器逐步增量移动。
 *
 * 工作原理：
 * - 定时器每 TIMER_INTERVAL_MS(50ms) 触发一次 onTimerTick
 * - 每次触发时计算增量角度：增量 = speed * 0.05（一秒内完成speed度）
 * - 逐步逼近目标角度，到达后标记 isMoving = false
 *
 * 示例：
 * - P01快（60度/秒）：setAngleWithSpeed(0, 90, 60)
 * - P02慢（15度/秒）：setAngleWithSpeed(1, 90, 15)
 * - 都转到90度，P01会更快到达
 *
 * @param channel 通道号 (0-5)，对应 P01-P06
 * @param angle 目标角度 (0-180度)，超出范围自动限幅
 * @param speed 转动速度（度/秒），范围 1-360，默认10度/秒
 * @return 设置成功返回true，通道无效返回false
 */
bool ServoController::setAngleWithSpeed(int channel, float angle, float speed)
{
    if (!checkChannel(channel)) return false;  // 通道合法性检查

    // 角度限幅: 确保在有效范围内
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    // 速度限幅: 1-360 度/秒（脚本模式速度范围）
    if (speed < MIN_SCRIPT_SPEED) speed = MIN_SCRIPT_SPEED;
    if (speed > MAX_SCRIPT_SPEED) speed = MAX_SCRIPT_SPEED;

    ServoState& st = m_states[channel];
    const float startAngle = st.currentAngle;
    const float delta      = angle - startAngle;
    const float absDelta   = qAbs(delta);

    st.targetAngle = angle;
    st.speed       = speed;

    // 阈值分流：短距离单帧直发（保留 2026-06-15 快速通道）
    if (absDelta <= SHORT_MOVE_THRESHOLD_DEG) {
//         qDebug() << "[ServoController] setAngleWithSpeed ch:" << channel
//                  << "from:" << startAngle << "to:" << angle
//                  << "speed:" << speed << "deg/s (short move, single-frame)";

        sendSingleFrame(channel, angle);

        st.currentAngle = angle;
        st.isMoving     = false;

        emit moveStarted(channel);
        emit angleChanged(channel, angle);
        emit moveCompleted(channel, angle);
        return true;
    }

    // 长距离：100ms tick 软插值（依赖 m_cachedRegs diff 缓存）
    const float durationSec = absDelta / speed;
    const int   totalTicks  = qBound(1, static_cast<int>(durationSec * (1000.0f / SOFT_INTERP_TICK_MS)), MAX_INTERP_TICKS);
    const float stepPerTick = delta / totalTicks;

//     qDebug() << "[ServoController] setAngleWithSpeed ch:" << channel
//              << "from:" << startAngle << "to:" << angle
//              << "speed:" << speed << "deg/s (long move,"
//              << totalTicks << "ticks x" << SOFT_INTERP_TICK_MS << "ms)";

    ChannelMovePlan plan;
    plan.channel     = channel;
    plan.targetAngle = angle;
    plan.stepPerTick = stepPerTick;
    plan.totalTicks  = totalTicks;
    plan.currentTick = 0;

    st.currentAngle = startAngle;  // 首 tick 推进 = startAngle + stepPerTick
    st.isMoving     = true;

    emit moveStarted(channel);
    emit angleChanged(channel, startAngle);

    if (!startSoftInterpolation({plan})) {
        // 启动失败时回退到单帧直发，避免 isMoving 卡死
        setError(QString("setAngleWithSpeed: 软插值启动失败, ch=%1").arg(channel));
        sendSingleFrame(channel, angle);
        st.currentAngle = angle;
        st.isMoving     = false;
        emit moveCompleted(channel, angle);
        return false;
    }
    return true;
}

/**
 * @brief 脚本模式批量下发舵机角度（单帧 6 路，避免 14 次同步 RPC 抖动）
 *
 * 修复（2026-06-15）：原 setAngleWithSpeed 走 sendRawModbusFrame → PUTCHAR 逐字节
 * 发送（8 字节/帧 × N 通道 = N×10 次 ZMC RPC），SETCOM 切换抖动 + UART 发送未
 * 排空导致多通道时第二路起丢帧。本方法改用 setAllPwm 单帧批量下发，避开抖动。
 *
 * 工作原理：
 *   - 构造 6 路占空比：传入通道用目标角度换算，其余通道用 m_states[ch].currentAngle
 *   - 调用 m_rs485->setAllPwm(0x21, 50, ...) → sendModbusRequest → MODBUSM_REGSET
 *   - m_cachedRegs diff 机制自动跳过未变通道，单次 RPC 完成 6 路下发
 *   - 更新 m_states[ch] 并 emit 信号（与 setAngleWithSpeed 时序一致）
 *
 * 典型调用：
 *   - SERVO(0=30:10)         → channels={0},    angles={30},    speeds={10}
 *   - SERVO(0=30:10,1=30,30) → channels={0,1},  angles={30,30}, speeds={10,30}
 *   - 满 6 通道同理
 *
 * @param channels  目标通道列表（0-5）
 * @param angles    目标角度列表（与 channels 一一对应）
 * @param speeds    速度列表（与 channels 一一对应；当前未使用）
 * @return 全部成功返回 true；入参不一致或任一通道非法返回 false
 */
bool ServoController::setAnglesForScript(const QList<int>& channels,
                                         const QList<float>& angles,
                                         const QList<float>& speeds)
{
    // Step 1: 入参一致性校验
    if (channels.size() != angles.size() || channels.size() != speeds.size()) {
        setError(QString("setAnglesForScript 参数不一致: channels=%1, angles=%2, speeds=%3")
                     .arg(channels.size()).arg(angles.size()).arg(speeds.size()));
        return false;
    }
    if (channels.isEmpty()) {
        return true;  // 幂等
    }
    if (!m_rs485) {
        setError("RS485控制器未设置");
        return false;
    }

    // Step 2: 逐通道合法性检查（checkChannel 内部会更新 m_lastError）
    for (int ch : channels) {
        if (!checkChannel(ch)) {
            setError(QString("setAnglesForScript 通道非法: %1").arg(ch));
            return false;
        }
    }

    // Step 3: 准备每通道计划
    QList<ChannelMovePlan> longPlans;     // 走软插值的长距离计划
    QList<int>             shortChannels; // 走单帧直发的短距离通道
    QList<float>           shortAngles;   // 短距离目标角度

    for (int i = 0; i < channels.size(); ++i) {
        const int   ch   = channels[i];
        float       ang  = angles[i];
        float       spd  = speeds.value(i, 10.0f);
        // 角度/速度限幅
        if (ang < MIN_ANGLE) ang = MIN_ANGLE;
        if (ang > MAX_ANGLE) ang = MAX_ANGLE;
        if (spd < MIN_SCRIPT_SPEED) spd = MIN_SCRIPT_SPEED;
        if (spd > MAX_SCRIPT_SPEED) spd = MAX_SCRIPT_SPEED;

        const float start   = m_states[ch].currentAngle;
        const float delta   = ang - start;
        const float absDelta = qAbs(delta);

        m_states[ch].targetAngle = ang;
        m_states[ch].speed       = spd;

        if (absDelta <= SHORT_MOVE_THRESHOLD_DEG) {
            // 短距离：单帧直发（保留快速通道）
            shortChannels << ch;
            shortAngles   << ang;
            m_states[ch].currentAngle = ang;
            m_states[ch].isMoving     = false;
        } else {
            // 长距离：构造软插值计划
            const float durationSec = absDelta / spd;
            const int   totalTicks  = qBound(1, static_cast<int>(durationSec * (1000.0f / SOFT_INTERP_TICK_MS)), MAX_INTERP_TICKS);
            const float stepPerTick = delta / totalTicks;

            ChannelMovePlan plan;
            plan.channel     = ch;
            plan.targetAngle = ang;
            plan.stepPerTick = stepPerTick;
            plan.totalTicks  = totalTicks;
            plan.currentTick = 0;
            longPlans << plan;

            m_states[ch].currentAngle = start;
            m_states[ch].isMoving     = true;
        }
    }

    // Step 4: 处理短距离通道——单帧批量下发 + emit 完成信号
    for (int i = 0; i < shortChannels.size(); ++i) {
        const int   ch = shortChannels[i];
        const float an = shortAngles[i];
        emit moveStarted(ch);
        emit angleChanged(ch, an);
        emit moveCompleted(ch, an);
    }
    if (!shortChannels.isEmpty()) {
        // 构造 6 路占空比一次性下发
        quint16 duties[6];
        for (int ch2 = 0; ch2 < m_channelCount; ++ch2) {
            duties[ch2] = angleToDutyPermil(m_states[ch2].currentAngle);
        }
        bool ok = m_rs485->setAllPwm(
            0x21,
            50, duties[0], 50, duties[1], 50, duties[2], 50, duties[3],
            duties[4], duties[5]);
        if (!ok) {
            setError("setAnglesForScript: 短距离 setAllPwm 失败");
            return false;
        }
    }

    // Step 5: 处理长距离通道——启动共享 100ms tick 软插值
    if (!longPlans.isEmpty()) {
        for (const ChannelMovePlan& p : longPlans) {
            emit moveStarted(p.channel);
            emit angleChanged(p.channel, m_states[p.channel].currentAngle);
        }
        if (!startSoftInterpolation(longPlans)) {
            // 启动失败时回退：所有长距离通道单帧直发
            setError("setAnglesForScript: 软插值启动失败，回退单帧直发");
            for (const ChannelMovePlan& p : longPlans) {
                sendSingleFrame(p.channel, p.targetAngle);
                m_states[p.channel].currentAngle = p.targetAngle;
                m_states[p.channel].isMoving     = false;
                emit moveCompleted(p.channel, p.targetAngle);
            }
            return false;
        }
    }
    return true;
}

/**
 * @brief 脚本模式批量下发舵机角度（速度/时间混合模式，2026-06-16 新增）
 *
 * 仿 KPZ32 出厂协议 `#xxxPxxxT<ms>!` 的语义：每路可独立选择
 * 速度（°/s）或时间（ms）控制。times[i] > 0 走时间分支，否则按 speeds[i] 走速度分支。
 *
 * 行为细节：
 *   - 短距离（|Δ| ≤ 30°）：单帧直发，物理层瞬时到位，times 参数被忽略
 *   - 长距离 + 时间模式（times[i] ≥ 20）：构造计划 totalTicks = times[i]/100ms，
 *     stepPerTick = Δ / totalTicks（与 KPZ32 公式 inc = Δ / (time/20) 思路一致，
 *     唯一区别是 tick 是 100ms 而不是 20ms——受 Qt 端 RPC 周期限制）
 *   - 长距离 + 时间模式（times[i] < 20）：单帧直发（对齐 KPZ32 `T<20` 行为）
 *   - 长距离 + 速度模式：与 setAnglesForScript 行为一致，times[i]==0 走速度分支
 *   - 所有长距离通道共享一个 100ms timer，每 tick 用 setAllPwm 同帧下发 6 路
 *
 * @param channels  目标通道列表（0-5）
 * @param angles    目标角度列表（与 channels 一一对应）
 * @param speeds    速度列表（与 channels 一一对应；时间模式下忽略）
 * @param times     时间列表（与 channels 一一对应；ms；0 走速度分支）
 * @return 全部成功返回 true
 */
bool ServoController::setAnglesForScriptMixed(const QList<int>& channels,
                                              const QList<float>& angles,
                                              const QList<float>& speeds,
                                              const QList<int>& times)
{
    // Step 1: 入参一致性校验
    if (channels.size() != angles.size() || channels.size() != speeds.size()
        || channels.size() != times.size()) {
        setError(QString("setAnglesForScriptMixed 参数不一致: channels=%1, angles=%2, speeds=%3, times=%4")
                     .arg(channels.size()).arg(angles.size()).arg(speeds.size()).arg(times.size()));
        return false;
    }
    if (channels.isEmpty()) {
        return true;  // 幂等
    }
    if (!m_rs485) {
        setError("RS485控制器未设置");
        return false;
    }

    // Step 2: 逐通道合法性检查
    for (int ch : channels) {
        if (!checkChannel(ch)) {
            setError(QString("setAnglesForScriptMixed 通道非法: %1").arg(ch));
            return false;
        }
    }

    // Step 3: 准备每通道计划（混合模式）
    QList<ChannelMovePlan> longPlans;
    QList<int>             shortChannels;
    QList<float>           shortAngles;
    QList<QString>         modeLog;  // 用于控制台回显（时间模式/速度模式）

    for (int i = 0; i < channels.size(); ++i) {
        const int   ch   = channels[i];
        float       ang  = angles[i];
        float       spd  = speeds.value(i, 10.0f);
        int         tMs  = times.value(i, 0);

        // 角度/速度限幅
        if (ang < MIN_ANGLE) ang = MIN_ANGLE;
        if (ang > MAX_ANGLE) ang = MAX_ANGLE;
        if (spd < MIN_SCRIPT_SPEED) spd = MIN_SCRIPT_SPEED;
        if (spd > MAX_SCRIPT_SPEED) spd = MAX_SCRIPT_SPEED;

        // 时间模式上限校验：超过 MAX_INTERP_TICKS * SOFT_INTERP_TICK_MS 时警告并截断
        const int MAX_TIME_MS = MAX_INTERP_TICKS * SOFT_INTERP_TICK_MS;
        if (tMs > MAX_TIME_MS) {
            qWarning() << "[ServoController] 时间参数超限: ch=" << ch
                       << "tMs=" << tMs << "→ 截断为" << MAX_TIME_MS;
            tMs = MAX_TIME_MS;
        }
        if (tMs < 0) tMs = 0;

        const float start   = m_states[ch].currentAngle;
        const float delta   = ang - start;
        const float absDelta = qAbs(delta);

        m_states[ch].targetAngle = ang;
        m_states[ch].speed       = spd;

        // 短距离：单帧直发（times 参数忽略）
        if (absDelta <= SHORT_MOVE_THRESHOLD_DEG) {
            shortChannels << ch;
            shortAngles   << ang;
            m_states[ch].currentAngle = ang;
            m_states[ch].isMoving     = false;
            modeLog << QStringLiteral("短距离直发");
            continue;
        }

        // 长距离 + 时间模式（tMs >= 20）：仿 KPZ32 inc = Δ / (time/20)
        if (tMs >= 20) {
            const int   totalTicks  = qBound(1, tMs / SOFT_INTERP_TICK_MS, MAX_INTERP_TICKS);
            const float stepPerTick = delta / totalTicks;

            ChannelMovePlan plan;
            plan.channel     = ch;
            plan.targetAngle = ang;
            plan.stepPerTick = stepPerTick;
            plan.totalTicks  = totalTicks;
            plan.currentTick = 0;
            longPlans << plan;

            m_states[ch].currentAngle = start;
            m_states[ch].isMoving     = true;
            modeLog << QStringLiteral("时间模式 T%1ms (%2 ticks × %3ms)").arg(tMs).arg(totalTicks).arg(SOFT_INTERP_TICK_MS);
            continue;
        }

        // 长距离 + 时间模式（tMs < 20）：单帧直发（对齐 KPZ32 `T<20` 行为）
        if (tMs > 0) {
            shortChannels << ch;
            shortAngles   << ang;
            m_states[ch].currentAngle = ang;
            m_states[ch].isMoving     = false;
            modeLog << QStringLiteral("时间模式 T%1ms (<20 瞬时)").arg(tMs);
            continue;
        }

        // 长距离 + 速度模式（tMs == 0）：与 setAnglesForScript 一致
        const float durationSec = absDelta / spd;
        const int   totalTicks  = qBound(1, static_cast<int>(durationSec * (1000.0f / SOFT_INTERP_TICK_MS)), MAX_INTERP_TICKS);
        const float stepPerTick = delta / totalTicks;

        ChannelMovePlan plan;
        plan.channel     = ch;
        plan.targetAngle = ang;
        plan.stepPerTick = stepPerTick;
        plan.totalTicks  = totalTicks;
        plan.currentTick = 0;
        longPlans << plan;

        m_states[ch].currentAngle = start;
        m_states[ch].isMoving     = true;
        modeLog << QStringLiteral("速度模式 %1°/s").arg(spd);
    }

    // Step 4: 调试日志（每路控制模式）
    for (int i = 0; i < channels.size(); ++i) {
//         qDebug() << "[ServoController] setAnglesForScriptMixed ch:" << channels[i]
//                  << "→" << angles[i] << "° 模式:" << modeLog.value(i);
    }

    // Step 5: 处理短距离通道——单帧批量下发 + emit 完成信号
    for (int i = 0; i < shortChannels.size(); ++i) {
        const int   ch = shortChannels[i];
        const float an = shortAngles[i];
        emit moveStarted(ch);
        emit angleChanged(ch, an);
        emit moveCompleted(ch, an);
    }
    if (!shortChannels.isEmpty()) {
        quint16 duties[6];
        for (int ch2 = 0; ch2 < m_channelCount; ++ch2) {
            duties[ch2] = angleToDutyPermil(m_states[ch2].currentAngle);
        }
        bool ok = m_rs485->setAllPwm(
            0x21,
            50, duties[0], 50, duties[1], 50, duties[2], 50, duties[3],
            duties[4], duties[5]);
        if (!ok) {
            setError("setAnglesForScriptMixed: 短距离 setAllPwm 失败");
            return false;
        }
    }

    // Step 6: 处理长距离通道——启动共享 100ms tick 软插值
    if (!longPlans.isEmpty()) {
        for (const ChannelMovePlan& p : longPlans) {
            emit moveStarted(p.channel);
            emit angleChanged(p.channel, m_states[p.channel].currentAngle);
        }
        if (!startSoftInterpolation(longPlans)) {
            // 启动失败时回退：所有长距离通道单帧直发
            setError("setAnglesForScriptMixed: 软插值启动失败，回退单帧直发");
            for (const ChannelMovePlan& p : longPlans) {
                sendSingleFrame(p.channel, p.targetAngle);
                m_states[p.channel].currentAngle = p.targetAngle;
                m_states[p.channel].isMoving     = false;
                emit moveCompleted(p.channel, p.targetAngle);
            }
            return false;
        }
    }
    return true;
}

/**
 * @brief 拖拽模式设置角度（仅更新内部状态，不发送）
 *
 * 直接更新内部状态并标记为移动中，由 onTimerTick 的批量帧统一发送。
 * 不单独发送 RAW 帧，避免与 onTimerTick 的批量发送冲突导致 P05/P06 滞后。
 */
bool ServoController::setAngleDrag(int channel, float angle)
{
    if (!checkChannel(channel)) return false;

    // 角度限幅
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    // 直接更新内部状态，不发送（由 onTimerTick 批量发送）
    m_states[channel].currentAngle = angle;
    m_states[channel].targetAngle = angle;
    m_states[channel].isMoving = true;  // 标记为移动中，触发批量发送

//     qDebug() << "[ServoController] setAngleDrag ch:" << channel << "angle:" << angle << "°";

    emit angleChanged(channel, angle);
    return true;
}

/**
 * @brief 设置默认速度(10度/秒)（仅更新内部状态，不发送
 */
void ServoController::setDefaultSpeed(int channel, float speed)
{
    if (!checkChannel(channel)) return;

    if (speed < 1.0f) speed = 1.0f;
    if (speed > 180.0f) speed = 180.0f;

    m_states[channel].speed = speed;
//     qDebug() << "[ServoController] setDefaultSpeed ch:" << channel << "speed:" << speed << "°/s";
}

/**
 * @brief 停止指定通道
 *
 * 停止该通道的平滑运动，并发送当前角度保持位置。
 */
bool ServoController::stop(int channel)
{
    if (!checkChannel(channel)) return false;

    m_states[channel].isMoving = false;
    // 保持在当前位置
    m_states[channel].targetAngle = m_states[channel].currentAngle;

    // 直接发送当前角度以保持位置（内联原 sendAngle 逻辑）
    quint8 pwmIndex = static_cast<quint8>(channel + 1);
    m_rs485->setServoAngle(pwmIndex, m_states[channel].currentAngle);

    return true;
}

/**
 * @brief 停止所有通道
 */
bool ServoController::stopAll()
{
    for (int i = 0; i < m_channelCount; ++i) {
        stop(i);
    }
    return true;
}

/**
 * @brief 初始化所有通道（设置默认频率50Hz，各舵机到指定角度）
 *
 * 初始角度：PWM1=122°, PWM2=113°, PWM3=49°, PWM4=109°, PWM5=75°, PWM6=80°
 * 通过 angleToDutyPermil() 自动计算占空比，无需手动换算。
 */
bool ServoController::initAll()
{
    if (!m_rs485) {
        setError("RS485控制器未设置");
        return false;
    }

    // 各舵机初始角度（度）
    const float initAngles[] = {122.0f, 113.0f, 49.0f, 109.0f, 75.0f, 80.0f};

//     qDebug() << "[ServoController] initAll: 设置舵机到指定角度"
//              << "PWM1=122° PWM2=113° PWM3=49° PWM4=109° PWM5=75° PWM6=80°";

    // 通过 angleToDutyPermil() 将角度转换为占空比千分比
    quint16 d1 = angleToDutyPermil(initAngles[0]);
    quint16 d2 = angleToDutyPermil(initAngles[1]);
    quint16 d3 = angleToDutyPermil(initAngles[2]);
    quint16 d4 = angleToDutyPermil(initAngles[3]);
    quint16 d5 = angleToDutyPermil(initAngles[4]);
    quint16 d6 = angleToDutyPermil(initAngles[5]);

//     qDebug() << "[ServoController] 占空比:" << d1 << d2 << d3 << d4 << d5 << d6;

    // 批量设置各舵机到指定角度
    bool ok = m_rs485->setAllPwm(
        0x21,           // 从站地址
        50, d1,         // PWM1: 50Hz, 占空比
        50, d2,         // PWM2: 50Hz, 占空比
        50, d3,         // PWM3: 50Hz, 占空比
        50, d4,         // PWM4: 50Hz, 占空比
        d5,             // PWM5: 占空比（与PWM1共享频率）
        d6              // PWM6: 占空比（与PWM1共享频率）
    );

    // 更新状态（各舵机独立初始角度）
    for (int i = 0; i < m_channelCount; ++i) {
        float angle = (i < 6) ? initAngles[i] : 90.0f;
        m_states[i].currentAngle = angle;
        m_states[i].targetAngle = angle;
        m_states[i].isMoving = false;
    }

    return ok;
}

/**
 * @brief 定时器回调，逐步更新角度（速度控制核心）
 *
 * 每隔 TIMER_INTERVAL_MS(50ms) 被定时器触发一次，实现舵机平滑转动。
 *
 * 工作流程：
 * 1. 检查是否有通道正在移动，无则直接返回
 * 2. 遍历所有通道，根据各通道设定的速度计算增量角度
 * 3. 逐步逼近目标角度，到达后标记该通道移动完成
 * 4. 收集所有通道当前角度，调用 setAllPwm 批量发送一次
 *
 * 关键设计：使用批量发送模式，所有通道的角度变化通过一次 Modbus 帧发送，
 * 避免逐通道单独发送导致的 P05/P06 滞后问题。
 */
void ServoController::onTimerTick()
{
    // Step 1: 检查是否有舵机在移动
    bool anyMoving = false;
    for (int i = 0; i < m_channelCount; ++i) {
        if (m_states[i].isMoving) {
            anyMoving = true;
            break;
        }
    }
    if (!anyMoving) return; // 无移动则直接返回，避免无效通信

    // Step 2: 收集各通道当前角度（用于后续批量发送）
    float angles[6];
    for (int i = 0; i < m_channelCount; ++i) {
        angles[i] = m_states[i].currentAngle;
    }

    // Step 3: 遍历所有通道，计算每通道的新角度
    for (int i = 0; i < m_channelCount; ++i) {
        if (!m_states[i].isMoving) continue;

        float currentAngle = m_states[i].currentAngle;
        float targetAngle = m_states[i].targetAngle;
        float speed = m_states[i].speed;

        // 计算与目标的角度差
        float diff = targetAngle - currentAngle;

        // 判断是否已到达目标（阈值0.5度，防止抖动）
        if (qAbs(diff) < 0.5f) {
            // 精确到达目标，标记移动完成
            m_states[i].currentAngle = targetAngle;
            m_states[i].isMoving = false;
            angles[i] = targetAngle;
            emit angleChanged(i, targetAngle);
            emit moveCompleted(i, targetAngle);
//             qDebug() << "[ServoController] ch" << i << "到达目标角度:" << targetAngle << "°";
            continue;
        }

        // 计算每50ms应转动的角度: 速度(度/秒) * 间隔(秒)
        float step = speed * (TIMER_INTERVAL_MS / 1000.0f);

        // 限制步进不超过剩余距离，防止过冲
        if (qAbs(step) > qAbs(diff)) {
            step = diff;
        }

        // 计算新角度并限幅
        float newAngle = currentAngle + step;
        if (newAngle < MIN_ANGLE) newAngle = MIN_ANGLE;
        if (newAngle > MAX_ANGLE) newAngle = MAX_ANGLE;

        // 更新状态并记录
        m_states[i].currentAngle = newAngle;
        angles[i] = newAngle;
        emit angleChanged(i, newAngle);
    }

    // Step 4: 批量发送所有6路舵机角度（一次Modbus帧完成，避免P05/P06滞后）
    if (m_rs485) {
        // 将角度转换为占空比千分比（复用 angleToDutyPermil，公式只维护一处）
        quint16 duties[6];
        for (int i = 0; i < m_channelCount; ++i) {
            duties[i] = angleToDutyPermil(angles[i]);
        }

        // 批量设置6路PWM（从站地址0x21，频率50Hz）
        m_rs485->setAllPwm(
            0x21,            // 从站地址
            50, duties[0],   // PWM1: 50Hz
            50, duties[1],   // PWM2: 50Hz
            50, duties[2],   // PWM3: 50Hz
            50, duties[3],   // PWM4: 50Hz
            duties[4],       // PWM5: 占空比（与PWM1共享频率）
            duties[5]        // PWM6: 占空比（与PWM1共享频率）
        );
    }
}

/**
 * @brief 单帧下发单通道目标角度（脚本模式私有辅助）
 *
 * 仅做一次 RS485 发送，不推进 currentAngle / isMoving（由调用方负责状态机）。
 * 调用前调用方应已对 angle 完成限幅。
 */
void ServoController::sendSingleFrame(int channel, float angle)
{
    if (!m_rs485) return;
    quint8 pwmIndex = static_cast<quint8>(channel + 1);
    m_rs485->setServoAngle(pwmIndex, angle);
}

/**
 * @brief 启动 100ms tick 软插值（共享 timer 多通道并发）
 *
 * 将传入的 plans 装入一个新 QTimer，每 SOFT_INTERP_TICK_MS(100ms) 推进一次：
 *   - 每 tick 推进每个 channel.stepPerTick
 *   - 每 tick 用 m_rs485->setAllPwm() 同帧下发 6 路（diff 缓存命中后 14-50ms）
 *   - 所有 channel 走完后 emit moveCompleted 并 deleteLater timer
 *
 * 新 timer 自动加入 m_activeTimers 列表，cancelAllMovements() 可一并清理。
 *
 * @param plans 软插值计划列表（不允许空）
 * @return 新创建的 QTimer 指针（失败返回 nullptr）
 */
QTimer* ServoController::startSoftInterpolation(const QList<ChannelMovePlan>& plans)
{
    if (plans.isEmpty()) return nullptr;
    if (!m_rs485) {
        setError("startSoftInterpolation: RS485 未设置");
        return nullptr;
    }

    auto* timer = new QTimer(this);
    timer->setInterval(SOFT_INTERP_TICK_MS);
    // plans 按值拷贝到 lambda
    QList<ChannelMovePlan> localPlans = plans;

    connect(timer, &QTimer::timeout, this, [this, timer, localPlans]() mutable {
        bool allDone = true;
        quint16 duties[6];
        for (int ch = 0; ch < m_channelCount; ++ch) {
            duties[ch] = angleToDutyPermil(m_states[ch].currentAngle);
        }

        for (ChannelMovePlan& p : localPlans) {
            if (p.currentTick >= p.totalTicks) continue;  // 已到位

            p.currentTick++;
            ServoState& st = m_states[p.channel];
            float newAngle = (p.currentTick >= p.totalTicks)
                          ? p.targetAngle
                          : st.currentAngle + p.stepPerTick;
            // 限幅
            if (newAngle < MIN_ANGLE) newAngle = MIN_ANGLE;
            if (newAngle > MAX_ANGLE) newAngle = MAX_ANGLE;
            st.currentAngle = newAngle;
            duties[p.channel] = angleToDutyPermil(newAngle);

            emit angleChanged(p.channel, newAngle);
            if (p.currentTick < p.totalTicks) allDone = false;
        }

        // 同帧下发 6 路（diff 缓存命中后 14-50ms，< 100ms tick）
        bool ok = m_rs485->setAllPwm(
            0x21,
            50, duties[0], 50, duties[1], 50, duties[2], 50, duties[3],
            duties[4], duties[5]);
        if (!ok) {
            qWarning() << "[ServoController] 软插值 tick 发送失败";
        }

        if (allDone) {
            for (const ChannelMovePlan& p : localPlans) {
                m_states[p.channel].isMoving = false;
                emit moveCompleted(p.channel, p.targetAngle);
            }
            m_activeTimers.removeAll(timer);
            timer->stop();
            timer->deleteLater();
        }
    });

    m_activeTimers << timer;
    timer->start();
    return timer;
}

/**
 * @brief 取消所有进行中的软插值（STOP/PAUSE 时调用）
 *
 * 停止所有活跃的 100ms 软插值 timer，将各通道 isMoving 复位为 false，
 * 降位是当前位置，targetAngle 回退到 currentAngle。便于
 * TaskExecutor::waitForServoIdle 立即返回。不会发送任何帧。
 */
void ServoController::cancelAllMovements()
{
    for (QTimer* t : m_activeTimers) {
        if (t) {
            t->stop();
            t->deleteLater();
        }
    }
    m_activeTimers.clear();

    for (int ch = 0; ch < m_channelCount; ++ch) {
        if (m_states[ch].isMoving) {
            m_states[ch].isMoving     = false;
            m_states[ch].targetAngle  = m_states[ch].currentAngle;
        }
    }
//     qDebug() << "[ServoController] cancelAllMovements: 全部软插值已取消";
}
