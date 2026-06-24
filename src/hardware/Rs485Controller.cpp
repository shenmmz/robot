/*
 * @file Rs485Controller.cpp
 * @brief RS485 Modbus通信控制器实现，通过ZMC控制器的Modbus转RS485功能发送数据。
 */
#include "Rs485Controller.h"
#include "../../ZmotionAPI/ZmotionAPI.h"
#include <QDebug>

Rs485Controller::Rs485Controller(QObject *parent)
    : QObject(parent)
{
}

Rs485Controller::~Rs485Controller()
{
}

/**
 * @brief 设置ZMotion API指针
 * @param api ZmotionAPI指针
 */
void Rs485Controller::setZmotionAPI(ZmotionAPI* api)
{
    m_zmotionAPI = api;
}

/**
 * @brief 设置错误信息
 * @param msg 错误信息
 */
void Rs485Controller::setError(const QString& msg)
{
    qWarning() << "[Rs485]" << msg;
    emit errorOccurred(msg);
}

/**
 * @brief 计算Modbus CRC16校验码
 * @param data 数据字节数组
 * @return CRC16校验码
 */
quint16 Rs485Controller::calcModbusCRC16(const QByteArray& data)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<quint8>(data[i]);
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

/**
 * @brief 在MODBUS_REG中写入一个16位值
 * @param api ZmotionAPI指针
 * @param index MODBUS_REG索引
 * @param value 16位值（已按大端序打包好的值）
 * @param errMsg 错误信息输出
 * @return 成功返回true，失败返回false
 */
static bool writeModbusReg(ZmotionAPI* api, int index, quint16 value, QString& errMsg)
{
    QString cmd = QString("MODBUS_REG(%1)=%2").arg(index).arg(value);
    ZmotionResult r = api->executeCommand(cmd);
    if (!r.success) {
        errMsg = r.errorMessage;
        return false;
    }
    return true;
}

/**
 * @brief 通过ZMC发送Modbus请求（ZMC内部自动组帧，含CRC/帧头）
 *
 * ZMC控制器负责组装完整的Modbus RTU帧并自动计算CRC，
 * 用户只需提供从站地址、功能码、起始地址和原始数据即可。
 *   1. SETCOM(9600,8,1,0,1,14,2,800) — 配置RS485端口1为Modbus主端，超时800ms
 *   2. MODBUSM_DES(从站地址, 端口) — 指定目标从站地址和通信端口
 *   3. 将数据以16位大端格式打包写入 MODBUS_REG
 *   4. MODBUSM_REGSET(startReg, wordCount, localRegStart) — 发送数据到从站
 *   5. MODBUSM_REGGET(startReg, wordCount, localRegReadStart) — 尝试读取从站响应
 *
 * 重要说明：
 *   - MODBUS_REG 是16位寄存器，2个字节必须打包成1个16位值（大端序）
 *   - MODBUSM_REGSET 的 num 参数是寄存器数量（16位为单位），不是字节数
 *   - 功能码由 ZMC 根据 num 自动选择（1个寄存器=0x06，多个=0x10）
 *   - 从站地址通过 MODBUSM_DES 指令设置，不是通过 SETCOM 设置
 *
 * @param slaveAddress 从站地址 (如 0x21)
 * @param functionCode 功能码（保留参数，当前由ZMC自动处理）
 * @param startAddress 从站寄存器起始地址
 * @param data 原始寄存器数据（每2字节=1个16位寄存器，大端序）
 * @return 发送成功返回true
 */
bool Rs485Controller::sendModbusRequest(quint8 slaveAddress, quint8 functionCode,quint16 startAddress, const QByteArray& data)
                                         
{
    if (!m_zmotionAPI) {
        setError(QStringLiteral("ZMotion API未设置"));
        return false;
    }

    // ============ 1. 显示要发送的原始数据 ============
    // qDebug().noquote() << QString("[Rs485] === 发送 Modbus 请求 ===");
    // qDebug().noquote() << QString("[Rs485]   从站地址: 0x%1").arg(slaveAddress, 2, 16, QChar('0'));
    // qDebug().noquote() << QString("[Rs485]   起始寄存器: 0x%1 (%2)")
    //                           .arg(startAddress, 4, 16, QChar('0')).arg(startAddress);
    // qDebug().noquote() << QString("[Rs485]   数据字节: %1").arg(data.toHex(' ').toUpper());

    // ============ 2. 配置RS485串口为Modbus主端（仅首次）============
    // SETCOM(波特率,数据位,停止位,校验,端口,模式,变量映射,超时ms)
    //   mode=14: Modbus主端(16位整数)
    //   variable=2: 系统MODBUS寄存器(缺省)
    //   timeout=800ms: ZMC手册建议800ms，避免过早超时
    ZmotionResult r;
    if (!m_serialConfigured) {
        r = m_zmotionAPI->executeCommand(
            QStringLiteral("SETCOM(9600,8,1,0,1,14,2,800)"));
        if (!r.success) {
            setError(QStringLiteral("SETCOM配置失败: %1").arg(r.errorMessage));
            return false;
        }
        m_serialConfigured = true;
        qDebug() << "[Rs485] SETCOM 配置成功 (9600,8,N,1, mode=14, timeout=800ms)";
    }

    // ============ 2.5. 指定从站地址（带缓存）============
    // MODBUSM_DES(从站地址, 端口) — 告诉ZMC通过哪个串口与哪个从站通信
    // 不调用此指令，ZMC不知道目标从站是谁，必然超时
    // 优化（2026-06-15）：ZMC 控制器会记忆从站地址，重复发 MODBUSM_DES 是冗余
    // RPC（每次约 5~20ms）。仅当从站变更时才发送。
    if (m_cachedSlaveAddress != slaveAddress) {
        QString desCmd = QString("MODBUSM_DES(%1,1)").arg(slaveAddress);
        qDebug().noquote() << "[Rs485] 设置从站:" << desCmd;
        r = m_zmotionAPI->executeCommand(desCmd);
        if (!r.success) {
            setError(QStringLiteral("MODBUSM_DES失败: %1").arg(r.errorMessage));
            return false;
        }
        m_cachedSlaveAddress = slaveAddress;
        qDebug().noquote() << QString("[Rs485] MODBUSM_DES 成功 (从站=0x%1, 端口=1)")
                                  .arg(slaveAddress, 2, 16, QChar('0'));
    } else {
        qDebug().noquote() << QString("[Rs485] 从站未变 (0x%1)，跳过 MODBUSM_DES")
                                  .arg(slaveAddress, 2, 16, QChar('0'));
    }

    // ============ 3. 将数据打包写入MODBUS_REG寄存器（diff 写入）============
    // data 是原始字节数组，需要按大端序打包成16位寄存器
    int byteCount = data.size();
    int wordCount = (byteCount + 1) / 2;  // 向上取整（奇数字节时最后一个寄存器只用高字节）

    qDebug().noquote() << QString("[Rs485]   数据打包: %1 字节 → %2 个16位寄存器")
                              .arg(byteCount).arg(wordCount);

    // 优化（2026-06-15）：diff 写入 — 与上次缓存相同的寄存器跳过，
    // 避免 12 次同步 RPC。典型场景（6 路 PWM 中只有 1 路改变）下，
    // RPC 数从 14 降到 1~3 次。
    int regWriteCount = 0;
    for (int i = 0; i < wordCount; i++) {
        int regIndex = m_localRegBase + i;
        // 大端序：高字节在前
        quint16 word = (static_cast<quint8>(data[i * 2]) << 8);
        if (i * 2 + 1 < byteCount) {
            word |= static_cast<quint8>(data[i * 2 + 1]);
        }

        // diff：与缓存相同则跳过（0xFFFF 哨兵表示未缓存过此索引）
        if (regIndex < m_cachedRegs.size()
            && m_cachedRegs.at(regIndex) == word) {
            continue;
        }

        QString errMsg;
        if (!writeModbusReg(m_zmotionAPI, regIndex, word, errMsg)) {
            setError(QStringLiteral("写入MODBUS_REG(%1)=%2失败: %3")
                           .arg(regIndex).arg(word).arg(errMsg));
            return false;
        }

        // 写后更新缓存
        if (regIndex >= m_cachedRegs.size()) {
            m_cachedRegs.resize(regIndex + 1);
        }
        m_cachedRegs[regIndex] = word;
        ++regWriteCount;
    }
    qDebug().noquote() << QString("[Rs485]   实际写入寄存器: %1 / %2 (diff 缓存生效)")
                              .arg(regWriteCount).arg(wordCount);

    // ============ 4. 发送 Modbus 主端写命令 ============
    // MODBUSM_REGSET(从站寄存器地址, 寄存器数量, 本地MODBUS_REG起始索引)
    // ZMC自动构建帧头（从站地址、功能码、CRC等），并发送到RS485总线
    QString sendCmd = QString("MODBUSM_REGSET(%1,%2,%3)")
                          .arg(startAddress)
                          .arg(wordCount)
                          .arg(m_localRegBase);
    qDebug() << "[Rs485] 执行:" << sendCmd;

    r = m_zmotionAPI->executeCommand(sendCmd);

    // ============ 5. 处理发送结果 ============
    if (!r.success) {
        qWarning().noquote() << QString("[Rs485] MODBUSM_REGSET 失败: %1").arg(r.errorMessage);

        // 判断是否为从站无响应超时
        if (r.errorMessage.contains(QStringLiteral("time out"), Qt::CaseInsensitive) ||
            r.errorMessage.contains(QStringLiteral("超时"), Qt::CaseInsensitive)) {
            qWarning() << "[Rs485] ⚠ 从站设备无响应！请检查:";
            qWarning() << "[Rs485]    1. PWM模块是否已上电？";
            qWarning() << "[Rs485]    2. RS485 A/B线是否接对 (A→A, B→B)？";
            qWarning() << QString("[Rs485]    3. 从站地址是否正确 (当前: 0x%1)？").arg(slaveAddress, 2, 16, QChar('0'));
            qWarning() << "[Rs485]    4. 波特率是否匹配 (9600,8,N,1)？";

            setError(QStringLiteral("从站无响应: %1").arg(r.errorMessage));
        } else {
            setError(QStringLiteral("Modbus发送失败: %1").arg(r.errorMessage));
        }
        emit communicationFailed(r.errorMessage);
        return false;
    }

    // ============ 6. 尝试读取从站响应 ============
    // MODBUSM_REGGET(从站寄存器地址, 寄存器数量, 本地MODBUS_REG起始索引)
    // 对于写命令，从站返回的是确认帧（与请求帧前6字节相同）
    // 注意: Modbus写命令通常不需要读取响应,发送成功即表示完成
    // 如果需要读取响应,应该使用异步方式或在工作线程中调用此函数
    
    // 对于Modbus写命令,ZMC的MODBUSM_REGSET已经处理了响应确认
    // 如果命令执行成功,说明从站已正确响应,无需再次读取
    qDebug() << "[Rs485] MODBUSM_REGSET 执行成功,从站已确认";
    emit communicationSuccess();// 发送成功信号
    emit dataSent(byteCount);// 发送完成信号，参数为实际发送的字节数
    return true;
}

/**
 * @brief 通过RAW模式+PUTCHAR发送完整Modbus RTU帧（用户自行组帧）
 *
 * 用户需要自行拼装完整的Modbus帧结构（地址+功能码+寄存器数+数据），
 * CRC由本函数自动计算追加。底层切换到RAW透传模式逐字节发送，发送完毕后自动恢复Modbus主端模式。
 *
 * 直接构建完整Modbus RTU帧（功能码0x10 - 写多个寄存器）：
 * [从站地址1B][功能码1B][起始地址2B][寄存器数2B][字节数1B][数据NB][CRC16 2B]
 * 
 * 实现步骤：
 *   1. 切换到RAW模式 (SETCOM mode=0)
 *   2. 使用PUTCHAR逐字节发送完整帧（包括CRC校验）
 *   3. 恢复Modbus主端模式 (SETCOM mode=14)
 *
 * @param slaveAddress 从站地址
 * @param functionCode 功能码 (通常为0x10)
 * @param startAddress 起始寄存器地址
 * @param data 寄存器数据字节数组（不含CRC，函数会自动添加）
 * @return 发送成功返回true
 */
bool Rs485Controller::sendRawModbusFrame(quint8 slaveAddress, quint8 functionCode,quint16 startAddress, const QByteArray& data)
{
                                          
    if (!m_zmotionAPI) {
        setError(QStringLiteral("ZMotion API未设置"));
        return false;
    }

    int byteCount = data.size();
    int wordCount = (byteCount + 1) / 2;

    // ============ 1. 构建完整Modbus RTU帧 ============
    // 帧格式: [地址1B][功能码1B][起始地址2B][寄存器数2B][字节数1B][数据NB][CRC 2B]
    QByteArray frame;
    frame.append(static_cast<char>(slaveAddress));
    frame.append(static_cast<char>(functionCode));
    frame.append(static_cast<char>((startAddress >> 8) & 0xFF));
    frame.append(static_cast<char>(startAddress & 0xFF));
    frame.append(static_cast<char>((wordCount >> 8) & 0xFF));
    frame.append(static_cast<char>(wordCount & 0xFF));
    frame.append(static_cast<char>(byteCount));
    frame.append(data);

    // 计算CRC16
    quint16 crc = calcModbusCRC16(frame);
    frame.append(static_cast<char>(crc & 0xFF));         // CRC低字节在前
    frame.append(static_cast<char>((crc >> 8) & 0xFF));  // CRC高字节在后

    qDebug().noquote() << QString("[Rs485] === RAW Modbus 帧 ===");
    qDebug().noquote() << QString("[Rs485]   完整帧(%1字节): %2")
                              .arg(frame.size())
                              .arg(frame.toHex(' ').toUpper());

    // ============ 2. 切换到RAW模式 ============
    ZmotionResult r = m_zmotionAPI->executeCommand(
        QStringLiteral("SETCOM(9600,8,1,0,1,0,2,800)"));
    if (!r.success) {
        setError(QStringLiteral("SETCOM RAW模式失败: %1").arg(r.errorMessage));
        return false;
    }
    qDebug() << "[Rs485] SETCOM → RAW模式 (9600,8,N,1)";

    // ============ 3. 逐字节发送 PUTCHAR ============
    for (int i = 0; i < frame.size(); i++) {
        quint8 byteVal = static_cast<quint8>(frame[i]);
        QString putcharCmd = QString("PUTCHAR#1,%1").arg(byteVal);
        r = m_zmotionAPI->executeCommand(putcharCmd);
        if (!r.success) {
            qWarning() << "[Rs485] PUTCHAR#" << i << "=" << byteVal << "失败:" << r.errorMessage;
        }
    }

    qDebug().noquote() << QString("[Rs485] RAW帧已发送 (%1 字节)").arg(frame.size());

    // ============ 4. 恢复Modbus主端模式 ============
    r = m_zmotionAPI->executeCommand(
        QStringLiteral("SETCOM(9600,8,1,0,1,14,2,800)"));
    if (r.success) {
        qDebug() << "[Rs485] SETCOM → 恢复Modbus主端模式";
    }

    emit dataSent(frame.size());
    emit communicationSuccess();// 发送成功信号
    return true;
}

/**
 * @brief 设置单个PWM输出的频率和占空比（单路PWM协议）
 *
 * 每次调用只设置1路PWM，使用规格书第5节第1小节的单路协议（寄存器地址0x2251~0x2256）。
 * 内部通过 sendRawModbusFrame() 发送，用户自行组帧。
 *
 * 寄存器地址：PWM1=0x2251, PWM2=0x2252, PWM3=0x2253, PWM4=0x2254, PWM5=0x2255, PWM6=0x2256
 * - 频率：3字节，大端序，范围1Hz~100KHz
 * - 占空比：2字节，大端序，千分比（0~1000表示0%~100%）
 * - PWM5/PWM6频率与PWM1相同，只设置占空比
 *
 * @param slaveAddress 从站地址 (默认0x21)
 * @param pwmIndex PWM输出索引 (1-6)
 * @param frequency PWM频率 (Hz)
 * @param dutyCycle PWM占空比 (0-1000，表示 0.00% - 100.00%)
 * @return true 发送成功，false 发送失败
 */
bool Rs485Controller::setSinglePwmFrequencyAndDuty(quint8 slaveAddress, quint8 pwmIndex,quint32 frequency, quint16 dutyCycle)                                             
{
    if (pwmIndex < 1 || pwmIndex > 6) {
        setError(QStringLiteral("无效的 PWM 索引 (1-6)"));
        return false;
    }

    // 普通协议寄存器地址：PWM1=0x2251, PWM2=0x2252, ..., PWM6=0x2256
    quint16 startAddress = 0x2250 + pwmIndex;

    // 保存PWM1的频率供PWM5/PWM6使用
    if (pwmIndex == 1) {
        m_pwm1Frequency = frequency;
    }

    // 实际使用的频率（PWM5/PWM6使用PWM1的频率）
    quint32 actualFreq = (pwmIndex >= 5 && pwmIndex <= 6) ? m_pwm1Frequency : frequency;

    // 构建数据（单路PWM协议）: [频率3字节] [占空比2字节] [填充1字节] = 6字节 = 3个16位寄存器
    // 规格书示例: 21 10 22 51 00 03 06 [00 27 10] [00 32] [00] 4E BB
    //             地址 码  起始  寄存器 字节  频率3B  占空比   填  CRC
    //                          数量    数              2B    充
    // 产品说明书规定：P05-P06 的频率和 P01 的频率相同，6 路占空比可单独设定。
    // 因此 PWM5/PWM6 必须下发与 PWM1 相同的频率字段（不能填 0 当作"不修改"，
    // 从站固件会把 0Hz 解释为真正的 0Hz，PWM 停摆）。
    // 修复（2026-06-15）：PWM5/PWM6 用 m_pwm1Frequency 填充频率字段。
    QByteArray dataToSend;
    quint32 effectiveFreq = (pwmIndex >= 5 && pwmIndex <= 6)
                            ? m_pwm1Frequency   // 与 PWM1 共享
                            : frequency;
    dataToSend.append(static_cast<char>((effectiveFreq >> 16) & 0xFF));  // 频率高字节
    dataToSend.append(static_cast<char>((effectiveFreq >> 8) & 0xFF));   // 频率中字节
    dataToSend.append(static_cast<char>(effectiveFreq & 0xFF));          // 频率低字节
    dataToSend.append(static_cast<char>((dutyCycle >> 8) & 0xFF));        // 占空比高字节
    dataToSend.append(static_cast<char>(dutyCycle & 0xFF));               // 占空比低字节
    dataToSend.append(static_cast<char>(0x00));                           // 填充

    qDebug() << "[Rs485] setSinglePwm: ch=" << pwmIndex
             << " freq=" << actualFreq << "Hz duty=" << dutyCycle
             << " data=" << dataToSend.toHex(' ').toUpper();

    // 使用RAW模式发送完整Modbus RTU帧（更可靠）
    return sendRawModbusFrame(slaveAddress, 0x10, startAddress, dataToSend);
}

/**
 * @brief 设置单个PWM占空比（使用默认地址和频率）
 *
 * 默认从站地址：0x21
 * 默认频率：50Hz（标准舵机频率）
 *
 * 特殊处理：
 *   - PWM1-4：直接设置频率和占空比
 *   - PWM5-6：与PWM1共享频率，仅设置占空比
 *   - 首次使用PWM5/6时，会自动初始化PWM1的频率为50Hz
 *
 * @param pwmIndex PWM通道索引 (1-6)
 * @param dutyCycle 占空比千分比 (0-1000)
 * @return true 发送成功，false 发送失败
 */
bool Rs485Controller::setPwmDuty(quint8 pwmIndex, quint16 dutyCycle)
{
    const quint8 defaultAddress = 0x21;
    const quint32 defaultFreq = 50;

    // 修复（2026-06-15）：移除"首次调 PWM5/6 自动初始化 PWM1"的哨兵逻辑
    // 旧实现用 m_lastDuty1==75 作为"未初始化"哨兵，存在被覆盖后无法触发的风险。
    // setSinglePwmFrequencyAndDuty 内部对 5/6 已直接用 m_pwm1Frequency
    // （构造时默认 50Hz）填充频率字段，无需先发一次 PWM1 初始化。
    return setSinglePwmFrequencyAndDuty(defaultAddress, pwmIndex, defaultFreq, dutyCycle);
}

/**
 * @brief 设置舵机角度（0-180度，自动转换为占空比）
 *
 * KPZ32舵机角度与占空比映射 (50Hz, 周期20ms)：
 * - 0度  -> 500us -> 2.5% 占空比 = 25/1000
 * - 90度 -> 1500us -> 7.5% 占空比 = 75/1000
 * - 180度-> 2500us -> 12.5%占空比 = 125/1000
 *
 * 公式与 ServoController::angleToDutyPermil() 一致。
 * 此处独立维护一份，因为 Rs485Controller 不依赖 ServoController。
 *
 * @param pwmIndex PWM通道索引 (1-6)
 * @param angle 舵机角度 (0-180度)
 * @return true 发送成功，false 发送失败
 */
bool Rs485Controller::setServoAngle(quint8 pwmIndex, float angle)// 舵机角度转占空比设置函数
{
    // 角度限幅: 0°~180°
    if (angle < 0.0f) angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    // KPZ32映射: duty = 25 + angle * (100 / 180)
    float dutyPermil = 25.0f + (angle / 180.0f) * 100.0f;
    quint16 dutyCycle = static_cast<quint16>(dutyPermil);
    // 发送PWM占空比设置命令
    return setPwmDuty(pwmIndex, dutyCycle);
}

/**
 * @brief 设置所有6路PWM（批量设置协议）
 *
 * 根据规格书第5节，批量设置寄存器起始地址为0x0246
 * 数据格式：PWM1-4各5字节(频率3B+占空比2B)，PWM5-6各2字节(仅占空比)
 * 总共：5*4 + 2*2 = 24字节 = 12个16位寄存器
 *
 * 特别说明：
 *   - PWM1-4：每路包含频率(3B)和占空比(2B)，共5字节
 *   - PWM5-6：仅包含占空比(2B)，因为它们与PWM1共享频率
 *   - 批量协议不需要填充字节（与单路协议不同）
 *
 * @param slaveAddress 从站地址
 * @param freq1-freq4 PWM1-PWM4的频率 (Hz)
 * @param duty1-duty6 PWM1-PWM6的占空比千分比 (0-1000)
 * @return true 发送成功，false 发送失败
 */
bool Rs485Controller::setAllPwm(quint8 slaveAddress,
                                 quint32 freq1, quint16 duty1,
                                 quint32 freq2, quint16 duty2,
                                 quint32 freq3, quint16 duty3,
                                 quint32 freq4, quint16 duty4,
                                 quint16 duty5, quint16 duty6)
{
    if (!m_zmotionAPI) {
        setError(QStringLiteral("ZMotion API未设置"));
        return false;
    }

    quint16 startAddress = 0x0246;// 批量设置寄存器起始地址

    QByteArray dataToSend;

    // PWM1: 频率(3) + 占空比(2) = 5字节 (批量协议无填充)
    dataToSend.append(static_cast<char>((freq1 >> 16) & 0xFF));// 频率高字节
    dataToSend.append(static_cast<char>((freq1 >> 8) & 0xFF));// 频率中字节
    dataToSend.append(static_cast<char>(freq1 & 0xFF));      // 频率低字节  ← 修复：之前缺失
    dataToSend.append(static_cast<char>((duty1 >> 8) & 0xFF));// 占空比高字节
    dataToSend.append(static_cast<char>(duty1 & 0xFF));      // 占空比低字节

    // PWM2: 频率(3) + 占空比(2) = 5字节
    dataToSend.append(static_cast<char>((freq2 >> 16) & 0xFF));
    dataToSend.append(static_cast<char>((freq2 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(freq2 & 0xFF));
    dataToSend.append(static_cast<char>((duty2 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(duty2 & 0xFF));

    // PWM3: 频率(3) + 占空比(2) = 5字节
    dataToSend.append(static_cast<char>((freq3 >> 16) & 0xFF));
    dataToSend.append(static_cast<char>((freq3 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(freq3 & 0xFF));
    dataToSend.append(static_cast<char>((duty3 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(duty3 & 0xFF));

    // PWM4: 频率(3) + 占空比(2) = 5字节
    dataToSend.append(static_cast<char>((freq4 >> 16) & 0xFF));
    dataToSend.append(static_cast<char>((freq4 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(freq4 & 0xFF));
    dataToSend.append(static_cast<char>((duty4 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(duty4 & 0xFF));

    // PWM5: 占空比(2) = 2字节
    dataToSend.append(static_cast<char>((duty5 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(duty5 & 0xFF));

    // PWM6: 占空比(2) = 2字节
    dataToSend.append(static_cast<char>((duty6 >> 8) & 0xFF));
    dataToSend.append(static_cast<char>(duty6 & 0xFF));

    // qDebug() << "[Rs485] setAllPwm: freq1=" << freq1 << " freq2=" << freq2
    //          << " freq3=" << freq3 << " freq4=" << freq4;

    return sendModbusRequest(slaveAddress, 0x10, startAddress, dataToSend);// 批量设置使用MODBUSM_REGSET发送
}
