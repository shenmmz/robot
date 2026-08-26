/*
 * @file ZmotionAPI.h
 * @brief 声明 ZmotionAPI 封装类和硬件通信相关的结果结构体。
 *
 * 本文件定义：
 * - ZmotionResult 统一返回结构
 * - AxisParamsCommand 参数枚举
 * - ZmotionAPI 类接口
 */
#ifndef ZMOTIONAPI_H
#define ZMOTIONAPI_H

#include <QString>
#include <QMap>
#include <QObject>
#include <QStringList>
#include "zauxdll2.h"
#include "zmotion.h"

/*
 * @brief ZmotionResults核心返回结构体，用于API调用的统一返回格式
 * 
 * 该结构体用于API调用的统一返回格式，包含执行状态、错误码和错误信息描述。
 * 所有API调用都应返回该结构体，方便调用者判断执行结果和处理错误。
 */
struct ZmotionResult {
    bool success;          // 执行状态：true表示成功，false表示失败
    int errorCode;         // 错误码：0表示成功，其他值为具体错误码
    QString errorMessage;  // 错误信息描述
    QStringList ipList;    // IP列表：用于存储所有IP地址
    QList<int> homeStatusList; //多轴的总线驱动器回零完成状态列表，与axes列表索引一一对应,0表示回零异常,1表示回零正常
    int piMultiValue; //多路输入信号，按位存储，每个位表示一个输入口的状态，0表示低电平，1表示高电平，一个 UNIT 存储32 个输入口状态。可读取 32 个及以上
    float axisPosition; //单轴当前位置
    int axisHomeStatus; //单轴总线驱动器回零完成状态，0表示回零异常,1表示回零正常
    float axisParamValue; //通用轴参数返回值，包含轴位置、轴速度、轴加速度、轴减速度、轴状态、轴错误码,使用ZAux_Direct_GetParam获取
    int axisCount; //控制器支持的轴数量

    /*
     * @brief 构造函数
     * 
     * @param s 执行状态，默认值为true表示成功
     * @param code 错误码，默认值为0表示成功
     * @param msg 错误信息描述，默认值为空字符串
     * @param ipList IP列表，默认值为空字符串列表
     * @param homeStatusList 多轴的总线驱动器回零完成状态列表，默认值为空整数列表
     * @param piMultiValue 多路输入信号，默认值为0
     * @param axisPosition 单轴当前位置，默认值为0.0
     */
    ZmotionResult(bool s = true, int code = 0, const QString& msg = "", const QStringList& ipList = QStringList(), const QList<int>& homeStatusList = QList<int>(), int piMultiValue = 0, float axisPosition = 0.0, int axisHomeStatus = 0)
        : success(s), errorCode(code), errorMessage(msg), ipList(ipList), homeStatusList(homeStatusList), piMultiValue(piMultiValue), axisPosition(axisPosition), axisHomeStatus(axisHomeStatus) {}     
};

enum  AxisParamsCommand {
    AType, //轴类型
    Units, //脉冲当量
    Accel, //加速度
    Decel, //减速度
    Speed, //运行速度
    Creep, //爬行速度
    Lspeed, //起始速度
    Merge, //连续插补开关
    Sramps, //S曲线-加减速曲线时间设置
    Dpos, //轴指令规划位置
    Mpos, //编码器反馈位置
    Endmove, //当前运动目标位置
    Fs_Limit, //正向软限位设置
    Rs_Limit, //负向软限位设置
    IDLE, //运动状态
    Axisstatus, //轴状态
    Axis_Enable, //轴使能开关
    Force_Speed,//sp 运动速度
    Startmove_Speed,//sp 运动开始速度
    Endmove_Speed,//sp 运动结束速度
    Fastdec,//快减减速度
};

// 使用 QMap 映射轴参数命令枚举到字符串
inline QMap<AxisParamsCommand, QString> axisParamsCommandToStringMap = {
    {AType, "Atype"}, //
    {Units, "Units"},
    {Accel, "Accel"},
    {Decel, "Decel"},
    {Speed, "Speed"},
    {Creep, "Creep"},
    {Lspeed, "Lspeed"},
    {Merge, "Merge"},
    {Sramps, "Sramps"},
    {Dpos, "Dpos"},
    {Mpos, "Mpos"},
    {Endmove, "Endmove"},
    {Fs_Limit, "Fs_Limit"},
    {Rs_Limit, "Rs_Limit"},
    {IDLE, "IDLE"},
    {Axisstatus, "Axisstatus"},
    {Axis_Enable, "Axis_Enable"},
    {Force_Speed, "Force_Speed"},
    {Startmove_Speed, "Startmove_Speed"},
    {Endmove_Speed, "Endmove_Speed"},
    {Fastdec, "Fastdec"},
    {Axisstatus, "Axisstatus"},
    {Axis_Enable, "Axis_Enable"},
    {Force_Speed, "Force_Speed"},
    {Startmove_Speed, "Startmove_Speed"},
    {Endmove_Speed, "Endmove_Speed"},
    {Fastdec, "Fastdec"},
};

/*
 * @brief 运动控制API类
 * 
 * 该类提供了与运动控制相关的API接口，包括初始化、运动控制、状态查询等功能。
 * 所有API调用都返回ZmotionResult结构体，方便调用者判断执行结果和处理错误。
 */
class ZmotionAPI : public QObject
{
    Q_OBJECT
    
public:
    // 错误码映射表（静态成员，用于存储所有错误码和对应描述）
    static QMap<int, QString> errorCodeMap;

    /*
     * @brief ZmotionAPI 构造函数
     * 
     * @param parent 父对象指针，默认值为nullptr
     */
    explicit ZmotionAPI(int axisMaxCount = 6, QObject *parent = nullptr);

    /*
     * @brief ~ZmotionAPI 析构函数
     */
    ~ZmotionAPI();

    /*
     * @brief isConnected 检查控制器是否已连接
     *
     * @return bool true表示已连接并初始化，false表示未连接
     */
    bool isConnected() const;

    // 控制详细日志输出，回零等批量操作时可关闭减少刷屏
    static void setVerbose(bool on) { s_verbose = on; }
    static bool isVerbose() { return s_verbose; }

    /*
     * @brief getErrorDescription 根据错误码获取错误描述
     *
     * @param errorCode 错误码
     * @return QString 错误描述
     */
    QString getErrorDescription(int errorCode);

    /********************* 机器人模型初始化 *********************************/
    /*
     * @brief setAtype 设置轴类型
     *
     * @param axis 轴编号
     * @param type 轴类型（0=虚拟轴, 1=旋转轴, 2=直线轴）
     * @return ZmotionResult 执行结果
     */
    ZmotionResult setAtype(int axis, int type);

    /*
     * @brief setTable 写入控制器 TABLE 数据
     *
     * @param tabStart TABLE 起始位置
     * @param count   写入数量
     * @param values  浮点数组
     * @return ZmotionResult 执行结果
     */
    ZmotionResult setTable(int tabStart, int count, const QVector<float>& values);
    ZmotionResult getTable(int tabStart, int count, QVector<float>& outValues);

    /********************* 综合应用函数 *********************************/
    /*
     * @brief startAxisSingleMoveAbs 单轴移动到绝对位置
     * 
     * @param axisNumber 轴编号
     * @param speed 速度
     * @param accel 加速度
     * @param decel 减速度
     * @param position 位置
     * @return ZmotionResult 执行结果
     */
    ZmotionResult startAxisSingleMoveAbs(int axisNumber, float speed, float accel, float decel, float position);
    
    /***************************控制卡基础控制***************************/

    /*
     * @brief searchEthlist 搜索网络中所有的ZMotion控制器
     * 
     * 该函数用于搜索网络中所有的ZMotion控制器，并返回它们的IP地址列表。
     * 搜索结果将存储在返回值的ipList字段中。
     * 
     * @return ZmotionResult 包含搜索结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         ipList存储所有搜索到的IP地址。
     */
    ZmotionResult searchEthlist();
    
    /*
     * @brief openEth 以太网连接控制器
     * 
     * 该函数用于通过以太网连接到指定IP地址的ZMotion控制器。
     * 成功连接后，控制器句柄将保存在内部的m_handle成员变量中。
     * 
     * @param ipaddr 连接的IP地址
     * @return ZmotionResult 包含连接结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult openEth(const QString& ipaddr);

    /*
     * @brief openCom 串口连接控制器
     *
     * 通过 RS232/RS485 串口连接 ZMotion 控制器。
     * 默认参数：38400 8N1（ZMotion 出厂默认）。
     *
     * @param comPort COM 口号（如 1 对应 COM1）
     * @param baudRate 波特率，默认 38400
     * @param dataBits 数据位，默认 8
     * @param parity 校验位：0=无, 1=奇, 2=偶，默认 0
     * @param stopBits 停止位：0=1位, 1=1.5位, 2=2位，默认 0
     * @return ZmotionResult 连接结果
     */
    ZmotionResult openCom(uint32 comPort, uint32 baudRate = 38400,
                          uint32 dataBits = 8, uint32 parity = 0, uint32 stopBits = 0);

    /*
     * @brief executeCommand 发送 ZBasic 在线命令
     *
     * 通过 ZAux_Execute 发送字符串命令到控制器，支持缓冲方式。
     * 示例命令："?VERSION", "?DPOS(0)", "RAPIDSTOP" 等。
     *
     * @param command ZBasic 命令字符串
     * @param response 用于接收控制器返回的响应字符串（可选）
     * @return ZmotionResult 执行结果
     */
    ZmotionResult executeCommand(const QString& command, QString* response = nullptr);
    
    /*
     * @brief close 关闭控制器连接
     * 
     * 该函数用于关闭当前与ZMotion控制器的连接。
     * 
     * @return ZmotionResult 包含关闭结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult close();

    /*
     * @brief getAxisCount 获取控制器支持的轴数量
     * 
     * 该函数用于获取控制器支持的总轴数（包括所有物理轴和虚拟轴）。
     * 使用 ZMC_GetAxises API实现。
     * 
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         axisCount字段存储控制器支持的轴数量
     */
    ZmotionResult getAxisCount();

    /*
     * @brief singleAxisCancel 单轴运动停止
     * 
     * 该函数用于停止指定轴的运动，支持多种停止模式。
     * 使用ZAux_Direct_Single_Cancel API实现运动停止控制。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param mode 停止模式：
     *        0 - 默认取消当前运动
     *        1 - 取消缓冲的运动
     *        2 - 取消当前运动和缓冲运动
     *        3 - 立即中断脉冲发送
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 如果指定轴在插补运动轴列表中，将停止轴组的插补运动
     * @note 停止后的绝对位置运动必须等待停止完成后再调用
     */
    ZmotionResult singleAxisCancel(const QList<int>& axes, int mode = 0);

    /*
     * @brief rapidStop 所有轴立即停止
     * 
     * 该函数用于停止所有轴的运动，如果轴参与插补运动，也会停止插补运动。
     * 使用ZAux_Direct_Rapidstop API实现所有轴的立即停止控制。
     * 
     * @param mode 停止模式：
     *        0 - 默认取消当前运动
     *        1 - 取消缓冲的运动
     *        2 - 取消当前运动和缓冲运动
     *        3 - 立即中断脉冲发送
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note Mode0~2 减速度按FASTDEC和DECEL中最大的值
     * @note RAPIDSTOP后要调用绝对位置运动，必须先WAIT IDLE等待停止完成
     */
    ZmotionResult rapidStop(int mode = 0);

    /*
     * @brief clearAxisPosition 轴位置清零和编码器清零
     * 
     * 该函数用于将指定轴的位置和编码器同时清零。
     * 使用ZAux_Direct_SetDpos设置指令位置为0，使用ZAux_Direct_SetMpos设置实际位置为0。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 请确保轴处于停止状态后再执行清零操作，避免运动中清零导致的安全问题
     */
    ZmotionResult clearAxisPosition(const QList<int>& axes);

    /*
     * @brief setAxisDpos 设置轴的指令位置（DPOS）
     *
     * 仅修改控制器内部位置计数器，不产生物理运动。
     * 使用 ZAux_Direct_SetDpos 实现。
     *
     * @param axes 轴号列表
     * @param positions 目标位置值列表，与axes一一对应
     * @return ZmotionResult 执行结果
     */
    ZmotionResult setAxisDpos(const QList<int>& axes, const QList<double>& positions);

    /*
     * @brief setAxisMpos 设置轴的编码器实际位置（MPOS）
     *
     * 仅修改控制器内部编码器位置计数器，不产生物理运动。
     * 使用 ZAux_Direct_SetMpos 实现。
     * 回零后恢复自定义原点时，需同时设置 DPOS 与 MPOS 为同一偏移值，
     * 否则闭环系统会因 DPOS/MPOS 不一致而驱动电机转动。
     *
     * @param axes 轴号列表
     * @param positions 目标实际位置值列表，与axes一一对应
     * @return ZmotionResult 执行结果
     */
    ZmotionResult setAxisMpos(const QList<int>& axes, const QList<double>& positions);

    /********************* 自定义原点（控制器 Flash 持久化）*******************/
    /*
     * @brief saveOriginToFlash 将原点偏移写入控制器用户 Flash（断电保持）
     *
     * 将 6 轴累计原点偏移写入控制器用户 Flash 块 0，
     * 数据格式：[魔数1, 魔数2, 轴数, 偏移0..偏移5]，共 9 个 float。
     * 断电后数据仍在，下次回零完成后由 readOriginFromFlash 读出并恢复。
     *
     * @param originOffsets 原点偏移列表（6 个元素，单位与轴坐标一致）
     * @return ZmotionResult 执行结果
     * @note Flash 写入有寿命限制（约10万次），仅在用户点击"原点设置"时调用
     */
    ZmotionResult saveOriginToFlash(const QList<double>& originOffsets);

    /*
     * @brief readOriginFromFlash 从控制器用户 Flash 读取原点偏移
     *
     * 读取 Flash 块 0 并校验魔数，校验通过则返回 6 轴累计原点偏移。
     *
     * @param originOffsets 输出参数，读取到的原点偏移列表
     * @return ZmotionResult success=true 表示 Flash 中存有有效原点数据
     *         （success=false 表示未设置过原点或数据无效，originOffsets 为空）
     */
    ZmotionResult readOriginFromFlash(QList<double>& originOffsets);

    /*
     * @brief clearOriginFlash 清除控制器用户 Flash 中的原点偏移
     *
     * 将 Flash 块 0 清零（魔数失效），恢复为"传感器原点"模式。
     *
     * @return ZmotionResult 执行结果
     */
    ZmotionResult clearOriginFlash();

    /*
     * @brief 获取单轴位置
     * 
     * 该函数用于获取单个轴的当前位置，使用ZAux_Direct_GetDpos API实现。
     * 
     * @param axis 轴号（0开始），支持单个轴号
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         axisPosition字段存储单轴当前位置
     * @note 单位为units
     */
    ZmotionResult getAxisPosition(int axis);

    /**
     * @brief 获取轴的通用参数值
     * 
     * 该函数用于获取单个轴的指定参数值，使用ZAux_Direct_GetParam API实现。
     * 
     * @param axis 轴号（0开始），支持单个轴号
     * @param paramType 参数类型，枚举类型AxisParamsCommand
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         axisParamValue字段存储轴参数值
     * @note 单位根据参数类型不同而不同，例如Speed为units/s，Accel为units/s^2等
     */
    ZmotionResult getAxisParamValue(int axis, AxisParamsCommand paramType);

    /**
     * @brief 设置轴的通用参数值
     * 
     * 该函数用于设置单个轴的指定参数值，使用ZAux_Direct_SetParam API实现。
     * 
     * @param axis 轴号（0开始），支持单个轴号
     * @param paramType 参数类型，枚举类型AxisParamsCommand
     * @param paramValue 参数值，根据参数类型不同而不同，例如Speed为units/s，Accel为units/s^2等
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 单位根据参数类型不同而不同，例如Speed为units/s，Accel为units/s^2等
     */
    ZmotionResult setAxisParamValue(int axis, AxisParamsCommand paramType, float paramValue);

    /***************************** 安全限制 ****************************************/
    /*
     * @brief setAxisFsLimit 设置轴的正软限位
     * 
     * 该函数用于设置单个或多个轴的正向软限位值。使用ZAux_Direct_SetFsLimit API实现。
     * 轴号列表和正向限位值列表的索引必须一一对应。
     * 
     * @param axes 轴号列表（0开始）
     * @param fsLimitValues 对应的正向限位值列表，单位为units
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 取消软限位时设置一个较大的值即可，建议设置为大于REP_DIST的值
     * @note 当FS_LIMIT大于REP_DIST时，参数不起作用，正向软限位被禁止
     * @note FS_LIMIT的默认值为200000000
     * @note 软限位无法作为DATUM回零时的限位信号参考
     * @note 设置限位前需检查有无设置对应的加减速以及快减速度，否则碰到限位无法停止
     */
    ZmotionResult setAxisFsLimit(const QList<int>& axes, const QList<float>& fsLimitValues);

    /*
     * @brief setAxisRsLimit 设置轴的负软限位
     * 
     * 该函数用于设置单个或多个轴的负向软限位值。使用ZAux_Direct_SetRsLimit API实现。
     * 轴号列表和负向限位值列表的索引必须一一对应。
     * 
     * @param axes 轴号列表（0开始）
     * @param rsLimitValues 对应的负向限位值列表，单位为units
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 取消软限位时设置一个较大的值即可，建议设置为大于REP_DIST的值
     * @note 当RS_LIMIT大于REP_DIST时，参数不起作用，负向软限位被禁止
     * @note RS_LIMIT的默认值为-200000000
     * @note 软限位无法作为DATUM回零时的限位信号参考
     * @note 设置限位前需检查有无设置对应的加减速以及快减速度，否则碰到限位无法停止
     */
    ZmotionResult setAxisRsLimit(const QList<int>& axes, const QList<float>& rsLimitValues);
    

    /************************** 运动控制参数 *******************************/

    /*
     * @brief setAxisUnits 设置轴的脉冲当量
     * 
     * 该函数用于设置指定轴的脉冲当量，支持同时设置多个轴。
     * 使用ZAux_Direct_SetUnits API实现脉冲当量的设置。
     * 脉冲当量是控制器基本单位，指定每单位发送的脉冲数，支持5位小数精度。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param units 脉冲当量值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 控制器以脉冲当量作为基本单位，修改后坐标显示会随脉冲当量改变比例变化
     * @note 请确保axes和units列表的长度相同，每个轴对应一个脉冲当量值
     * @note 示例：电机U=3600脉冲转一圈，丝杠螺距P=2mm，工作台走1mm对应的脉冲当量为：U/P=3600/2=1800
     */
    ZmotionResult setAxisUnits(const QList<int>& axes, const QList<double>& units);

    /*
     * @brief setAxisEnable 设置轴的使能状态
     *
     * 该函数用于启用或禁用指定轴。
     * 使用ZAux_Direct_SetAxisEnable API实现。
     *
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param enable 使能状态，1表示启用，0表示禁用
     * @return ZmotionResult 包含执行结果的结构体
     */
    ZmotionResult setAxisEnable(const QList<int>& axes, int enable);

    /*
     * @brief setAxisAccel 设置轴的加速度
     * 
     * 该函数用于设置指定轴的加速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetAccel API实现加速度的设置。
     * 加速度的单位为units/s/s，其中units为脉冲当量单位。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param accelerations 加速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 当多轴运动时，插补运动的加速度设置主轴加速度（主轴为轴列表中的第0个轴）
     * @note 建议在运动前设置好加速度，运动中不要修改，否则会导致速度曲线变化
     * @note 请确保axes和accelerations列表的长度相同，每个轴对应一个加速度值
     */
    ZmotionResult setAxisAccel(const QList<int>& axes, const QList<double>& accelerations);

    /*
     * @brief setAxisDecel 设置轴的减速度
     * 
     * 该函数用于设置指定轴的减速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetDecel API实现减速度的设置。
     * 减速度的单位为units/s/s，其中units为脉冲当量单位。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param decelerations 减速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 当多轴运动时，插补运动的减速度设置主轴减速度（主轴为轴列表中的第0个轴）
     * @note 建议在运动前设置好减速度，运动中不要修改，否则会导致速度曲线变化
     * @note 当设置减速度为0时，自动等于加速度ACCEL值，进行对称的加减速
     * @note 请确保axes和decelerations列表的长度相同，每个轴对应一个减速度值
     */
    ZmotionResult setAxisDecel(const QList<int>& axes, const QList<double>& decelerations);

    /*
     * @brief setAxisSpeed 设置轴的速度
     * 
     * 该函数用于设置指定轴的速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetSpeed API实现速度的设置。
     * 速度的单位为units/s，其中units为脉冲当量单位。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param speeds 速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult setAxisSpeed(const QList<int>& axes, const QList<double>& speeds);

    /*
     * @brief setAxisFastDecel 设置轴的快速减速度
     * 
     * 该函数用于设置指定轴的快速减速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetFastDec API实现快速减速度的设置。
     * 快速减速度的单位为units/s/s，其中units为脉冲当量单位。
     * 快速减速度在CANCEL或异常停止时自动采用。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param fastDecelerations 快速减速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 当设置快速减速度为0时，自动等于减速度DECEL值
     * @note 快速减速度通常在紧急停止时使用，建议设置为较大的值以确保快速响应
     * @note 请确保axes和fastDecelerations列表的长度相同，每个轴对应一个快速减速度值
     */
    ZmotionResult setAxisFastDecel(const QList<int>& axes, const QList<double>& fastDecelerations);

    /*
     * @brief setAxisLspeed 设置轴的起始速度
     * 
     * 该函数用于设置指定轴的起始速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetLspeed API实现起始速度的设置。
     * 起始速度的单位为units/s，其中units为脉冲当量单位。
     * 起始速度是运动开始时的初始速度，影响运动的效率和平滑性。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param speeds 起始速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 当多轴运动时，插补运动的起始速度设置主轴起始速度（主轴为轴列表中的第0个轴）
     * @note 当需要追求效率时，可以考虑设置合适的起始速度
     * @note 请确保axes和speeds列表的长度相同，每个轴对应一个起始速度值
     */
    ZmotionResult setAxisLspeed(const QList<int>& axes, const QList<double>& speeds);

    /*
     * @brief setAxisSramp 设置轴的S曲线时间
     * 
     * 该函数用于设置指定轴的S曲线时间，支持同时设置多个轴。
     * 使用ZAux_Direct_SetSramp API实现S曲线时间的设置。
     * S曲线时间的单位为毫秒(ms)，当值为0时表示使用梯形加减速。
     * S曲线时间会影响运动的平滑性，设置较大的值会使加减速过程更加平滑，但会延长加减速时间。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param srampValues S曲线时间值列表，与axes列表索引一一对应，单位为ms
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note S曲线时间的最大值不应超过原来T型加速度的加速时间
     * @note 设置S曲线时间后，加减速过程会延长相应的时间
     * @note 值为0时采用梯形加减速，值越大平滑度越高但加减速时间越长
     * @note 请确保axes和srampValues列表的长度相同，每个轴对应一个S曲线时间值
     */
    ZmotionResult setAxisSramp(const QList<int>& axes, const QList<double>& srampValues);

    /******************************单轴运动*******************************/
    /*
     * @brief setAxisSingleMove 设置轴的相对运动
     * 
     * 该函数用于执行指定轴的相对运动，支持同时对多个轴执行相对运动。
     * 使用ZAux_Direct_Single_Move API实现单轴相对运动控制。
     * 相对运动是指轴从当前位置移动指定距离，距离的正负决定运动方向。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param distances 相对运动距离值列表，与axes列表索引一一对应，单位为脉冲当量单位
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 相对运动的距离值可正可负，正数表示正向运动，负数表示反向运动
     * @note 在执行运动前，请确保已正确设置轴的速度、加速度和减速度等参数
     * @note 对于多轴同时运动，各轴将独立执行自己的相对运动，不会进行插补
     * @note 请确保axes和distances列表的长度相同，每个轴对应一个距离值
     */
    ZmotionResult setAxisSingleMove(const QList<int>& axes, const QList<double>& distances);

    /*
     * @brief setAxisSingleMoveAbs 设置轴的绝对运动
     * 
     * 该函数用于执行指定轴的绝对运动，支持同时对多个轴执行绝对运动。
     * 使用ZAux_Direct_Single_MoveAbs API实现单轴绝对运动控制。
     * 绝对运动是指轴从当前位置移动到指定的绝对位置。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param positions 绝对运动位置值列表，与axes列表索引一一对应，单位为脉冲当量单位
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 绝对运动的位置值是相对于零点的目标位置
     * @note 在执行运动前，请确保已正确设置轴的速度、加速度和减速度等参数     * @note 对于多轴同时运动，各轴将独立执行自己的绝对运动，不会进行插补
     * @note 请确保axes和positions列表的长度相同，每个轴对应一个位置值
     */
    ZmotionResult setAxisSingleMoveAbs(const QList<int>& axes, const QList<double>& positions);

    /**
     * @brief 单轴或多轴持续运动控制
     * @param axes 轴号列表，支持单轴或多轴控制
     * @param directions 方向列表，-1表示负向，1表示正向，与axes列表索引一一对应
     * @return ZmotionResult 操作结果，包含错误码和错误信息
     * @note 1. 当多个轴需要同时运动时，请确保axes和directions列表长度相同
     *       2. 如果某个轴正在进行Vmove运动，新的Vmove指令会自动替换之前的指令，无需取消前一个指令
     *       3. 轴号必须在控制器支持的有效范围内
     *       4. 方向参数必须为-1或1，其他值会导致错误
     */
    ZmotionResult setAxisSingleVmove(const QList<int>& axes, const QList<int>& directions);

    /*****************************回零运动******************************/

    /**
     * @brief setAxisDatumOrigin 设置轴的映射原点
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param originsInValue IO口编号，映射原点值列表，与axes列表索引一一对应，-1表示取消映射原点
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 映射原点是轴的参考位置，用于回零运动和绝对运动。
     *       设置映射原点后，轴将从该位置开始运动。
     *       -1 表示取消映射原点，轴将从当前位置开始运动。
     * @note 请确保axes和originsInValue列表的长度相同，每个轴对应一个映射原点值
     */
    ZmotionResult setAxisDatumOrigin(const QList<int>& axes, const QList<int>& originsInValue);

    /*
     * @brief setAxisCreepSpeed 设置轴的爬行速度
     * 
     * 该函数用于设置指定轴的爬行速度，支持同时设置多个轴。
     * 使用ZAux_Direct_SetCreep API实现爬行速度的设置。
     * 爬行速度用于原点搜寻，单位为units/s，其中units为脉冲当量单位。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param speeds 爬行速度值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 请确保axes和speeds列表的长度相同，每个轴对应一个爬行速度值
     * @note 爬行速度是原点搜寻过程中的低速运动速度，通常设置为较小的值以确保定位精度
     */
    ZmotionResult setAxisCreepSpeed(const QList<int>& axes, const QList<double>& speeds);

    /*
     * @brief setAxisDatumOffset 设置轴的回零偏移距离
     * 
     * 该函数用于设置指定轴的回零偏移距离，支持同时设置多个轴。
     * 使用ZAux_BusCmd_SetDatumOffpos API实现回零偏移距离的设置。
     * 回零偏移距离用于在回零完成后对轴的位置进行偏移调整。
     * 注意：主要针对总线电机，脉冲型电机的支持情况由固件决定。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param offsets 回零偏移距离值列表，与axes列表索引一一对应
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult setAxisDatumOffset(const QList<int>& axes, const QList<double>& offsets);

    /**
     * @brief setAxisSingleDatum 启动控制器轴单轴回零-自定义二次回零
     * 
     * @param axisIndex 轴号
     * @param homeMode 回零模式
     * @return ZmotionResult 操作结果
     * @return HihSpeed 回零高速：回零时的高速运动速度，单位为脉冲当量单位/s
     * @return LowSpeed 回零低速：回零时的低速运动速度，单位为脉冲当量单位/s
     * @return Datumoffset 回零偏移距离：回零完成后对轴的位置进行偏移调整的距离，单位为脉冲当量单位
     */
    ZmotionResult setAxisSingleDatum(int axisIndex, int homeMode, float HighSpeed, float LowSpeed, float Datumoffset);

    /**
     * @brief getAxisSingleHomeStatus 获取轴单轴回零状态
     * 
     * 该函数用于获取指定轴的单轴回零状态，支持同时获取多个轴的状态。
     * 使用ZAux_Direct_GetHomeStatus API实现单轴回零状态获取功能。
     * 返回的ZmotionResult结构体中的homeStatusList将包含各轴的回零状态，
     * 索引与输入的axes列表一一对应，0表示回零异常，1表示回零正常。
     * 
     * @param axisIndex 轴号
     * @return ZmotionResult 操作结果
     */
    ZmotionResult getAxisSingleHomeStatus(int axisIndex);

    /*
     * @brief startAxisBusDatum 启动轴的总线驱动器回零
     * 
     * 该函数用于启动指定轴的总线驱动器回零操作，支持同时启动多个轴。
     * 使用ZAux_BusCmd_Datum API实现总线驱动器回零功能。
     * 回零时会使用轴的SPEED, CREEP, ACCEL, DECEL参数乘以UNITS后自动设置驱动器的参数。
     * 动作时序：6098回零方式→6099速度→609A加速度→6060切换当前模式
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @param homeModes 回零模式列表，与axes列表索引一一对应，0表示使用驱动器当前的回零方式
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult startAxisBusDatum(const QList<int>& axes, const QList<int>& homeModes);

    /*
     * @brief getAxisBusHomeStatus 获取轴的总线驱动器回零完成状态
     * 
     * 该函数用于获取指定轴的总线驱动器回零完成状态，支持同时获取多个轴的状态。
     * 使用ZAux_BusCmd_GetHomeStatus API实现总线驱动器回零状态获取功能。
     * 返回的ZmotionResult结构体中的homeStatusList将包含各轴的回零状态，
     * 索引与输入的axes列表一一对应，0表示回零异常，1表示回零正常。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴号
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         homeStatusList包含各轴的回零状态（0-回零异常，1-回零正常）
     */
    ZmotionResult getAxisBusHomeStatus(const QList<int>& axes);

    /*******************************插补运动***************************************/
    /**
     * @brief directSetMerge 设置单轴的连续插补开关
     * @param axis 轴号
     * @param enable true-打开连续插补，false-关闭连续插补
     * @return ZmotionResult 包含执行状态、错误码和错误信息
     * @note 连续插补功能允许多段插补之间不减速，提高运动效率
     *       但在某些情况下即使设置为ON也可能仍然减速，请参考API手册
     */
    ZmotionResult directSetMerge(int axis, bool enable);

    /**
     * @brief directSetCornerMode 设置拐角模式
     * @param axis 轴号（主轴）
     * @param mode 模式设置值，不同位代表不同功能（可组合使用）：
     *        - 0位(值1): 预留
     *        - 1位(值2): 自动拐角减速（按ACCEL,DECEL加减速度）
     *        - 2位(值4): 预留
     *        - 3位(值8): 自动小圆限速（半径小于设置值时限速）
     *        - 4位(值16): 预留
     *        - 5位(值32): 自动倒角设置（与前面的MOVE运动做倒角处理）
     * @return ZmotionResult 包含执行状态、错误码和错误信息
     * @note 此函数只需设置主轴即可，在MOVE函数调用前生效
     *       减速拐角参考速度以FORCE_SPEED速度为参考
     *       小圆限速按公式：限速=FORCE_SPEED * 实际半径/FULL_SP_RADIUS
     */
    ZmotionResult directSetCornerMode(int axis, int mode);

    /*
     * @brief directMoveAbsolute 绝对直线插补运动
     * 
     * 该函数用于执行多轴的绝对直线插补运动。插补运动时只有主轴速度参数有效，
     * 主轴是轴列表中的第一个轴，运动参照这个轴的参数。
     * 插补运动距离 X=√(X₀²+X₁²+X₂²+…+Xₙ²)，运动时间 T=X/主轴SPEED
     * 使用ZAux_Direct_SetSpeed、ZAux_Direct_SetAccel、ZAux_Direct_SetDecel和ZAux_Direct_MoveAbs API实现功能。
     * 
     * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
     * @param distances 绝对距离列表，与axes列表索引一一对应
     * @param speed 插补速度值，应用于主轴（第一个轴）
     * @param accel 插补加速度值，应用于主轴（第一个轴）
     * @param decel 插补减速度值，应用于主轴（第一个轴）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult directMoveAbsolute(const QList<int>& axes, const QList<float>& distances, 
                                     float speed, float accel, float decel);

    /**
     * @brief directMoveAbsoluteSP 绝对直线插补运动-带SP
     * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
     * @param distances 绝对距离列表，与axes列表索引一一对应
     * @param startSpeed 开始速度值，应用于主轴（第一个轴）
     * @param endSpeed 结束速度值，应用于主轴（第一个轴）
     * @param forceSpeed 强制速度值，应用于主轴（第一个轴）
     * @param accel 插补加速度值，应用于主轴（第一个轴）
     * @param decel 插补减速度值，应用于主轴（第一个轴）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     */
    ZmotionResult directMoveAbsoluteSP(const QList<int>& axes, const QList<float>& distances, 
                                     float startSpeed, float endSpeed, float forceSpeed, float accel, float decel);

    /**
     * @brief directMultiMoveAbsolute 多条多轴直线绝对直线插补运动
     * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
     * @param distancesList 多段运动的距离列表，每个子列表表示一段运动的距离，与axes列表索引一一对应
     * @param speed 插补速度值，应用于主轴（第一个轴）
     * @param accel 插补加速度值，应用于主轴（第一个轴）
     * @param decel 插补减速度值，应用于主轴（第一个轴）
     * @return ZmotionResult 包含执行状态、错误码和错误信息
     * @note 此函数用于一次性发送多段多轴绝对位置运动指令
     */
    ZmotionResult directMultiMoveAbsolute(const QList<int>& axes, const QList<QList<float>>& distancesList,
                                        float speed, float accel, float decel);
    
    /*
     * @brief directMoveOp 设置输出插补指令
     * 
     * 该函数用于将指定的输出IO写入到运动缓冲中，在运动过程中按顺序执行IO操作。
     * 使用ZAux_Direct_MoveOp API实现。
     * 
     * @param axis 轴号（0开始），为主轴轴号
     * @param ioNumber IO编号
     * @param ioValue IO状态（0或1）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 此函数只对主轴轴号有效
     * @note IO操作会被添加到运动缓冲中，按照添加的顺序在运动过程中执行
     * @note 示例：轴0运动完成后，将输出1设置为1状态
     */
    ZmotionResult directMoveOp(int axis, int ioNumber, int ioValue);

    /*
     * @brief directMoveDelay 设置延时插补指令
     * 
     * 该函数用于在运动缓冲中写入延时指令，使控制器在执行完前面的运动指令后等待指定时间。
     * 使用ZAux_Direct_MoveDelay API实现。
     * 
     * @param axis 轴号（0开始），为主轴轴号
     * @param delayMs 延时时间，单位为毫秒（ms）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 此函数只对主轴轴号有效
     * @note 延时指令会被添加到运动缓冲中，在执行完前面的运动指令后等待指定时间
     * @note 前面的运动指令结束时速度会自动降为0
     * @note 示例：轴0运动完成后，延时1000毫秒（1秒），才执行下一条缓冲指令
     */
    ZmotionResult directMoveDelay(int axis, int delayMs);

    /******************************** 数字输入输出 **************************************/
    /*
     * @brief setOp 设置单个输出口信号
     * 
     * 该函数用于设置单个输出口的状态，使用ZAux_Direct_SetOp API实现。
     * 
     * @param ioNumber 输出口编号
     * @param ioValue 输出口状态值（0或1）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 输出口编号要大于控制器自身最大的IO编号，不能与控制器的编号重合
     * @note 使用示例：设置输出口1为1状态
     */
    ZmotionResult setOp(int ioNumber, int ioValue);

    /*
     * @brief getDI 获取单个输入口信号
     * 
     * 该函数用于获取单个输入口的状态，使用ZAux_Direct_GetIn API实现。
     * 
     * @param ioNumber 输入口编号
     * @return int 输入口状态：0表示低电平，1表示高电平，-1表示获取失败
     * @note 输入口编号要大于控制器自身最大的IO编号，不能与控制器的编号重合
     */
    int getDI(int ioNumber);

    /*
     * @brief getDO 获取单个输出口信号
     * 
     * 该函数用于获取单个输出口的状态，使用ZAux_Direct_GetOp API实现。
     * 
     * @param ioNumber 输出口编号
     * @return 输出口状态值（0或1），失败返回 -1
     * @note 使用示例：获取输出口1的状态
     */
    int getDO(int ioNumber);
    
    /**
     * @brief setInvertIn 设置输入IO的反转状态
     * 
     * 该函数用于设置指定输入IO口的电平反转状态。
     * 使用ZAux_Direct_SetInvertIn API实现。
     * 
     * @param ioNumber IO口编号
     * @param invert 反转状态：0表示不反转，1表示反转
     * @return ZmotionResult 包含执行结果的结构体
     */
    ZmotionResult setInvertIn(int ioNumber, int invert);

    /*
     * @brief getInMulti 获取多路输入口信号
     * 
     * 该函数用于获取多路输入口的状态，使用ZAux_Direct_GetInMulti API实现。
     * 
     * @param startIo 起始IO口编号，默认值为0
     * @param endIo 结束IO口编号，默认值为8
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述，
     *         piMultiValue存储获取到的IO口状态（按位存储，每个位表示一个输入口的状态）
     * @note 多路输入口信号按位存储，一个UNIT存储32个输入口状态
     */
    ZmotionResult getInMulti(int startIo = 0, int endIo = 8);
    
    /**
     * @brief setMoveOp 设置输出插补指令
     * 
     * 该函数用于将指定的输出IO写入到运动缓冲中，在运动过程中按顺序执行IO操作。
     * 使用ZAux_Direct_MoveOp API实现。
     * 
     * @param axis 轴号（0开始），为主轴轴号
     * @param ioNumber IO编号
     * @param ioValue IO状态（0或1）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 此函数只对主轴轴号有效
     * @note IO操作会被添加到运动缓冲中，按照添加的顺序在运动过程中执行
     * @note 示例：轴0运动完成后，将输出1设置为1状态
     */
    ZmotionResult setMoveOp(int axis, int ioNumber, int ioValue);

    /**
     * @brief setMoveDelay 设置延时插补指令
     * 
     * 该函数用于在运动缓冲中写入延时指令，使控制器在执行完前面的运动指令后等待指定时间。
     * 使用ZAux_Direct_MoveDelay API实现。
     * 
     * @param axis 轴号（0开始），为主轴轴号
     * @param delayMs 延时时间，单位为毫秒（ms）
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 此函数只对主轴轴号有效
     * @note 延时指令会被添加到运动缓冲中，在执行完前面的运动指令后等待指定时间
     * @note 前面的运动指令结束时速度会自动降为0
     * @note 示例：轴0运动完成后，延时1000毫秒（1秒），才执行下一条缓冲指令
     */
    ZmotionResult setMoveDelay(int axis, int delayMs);

private:
    ZMC_HANDLE m_handle = NULL;    // 设备句柄
    bool m_initialized = false;     // 初始化状态
    int m_axisMaxCount = 6;            // 最大轴数
    int m_currentMotionMode = -1;      // 当前运动模式：-1=未设置, 0=正解模式, 1=逆解模式
    static bool s_verbose;             // 是否输出详细日志
    
    /*
     * @brief initializeErrorCodeMap 初始化错误码映射表
     * 
     * 该函数用于初始化错误码映射表，将所有可能的错误码和对应的描述添加到映射表中。
     * 该函数在类构造函数中调用，确保映射表在使用前已经初始化。
     */
    static void initializeErrorCodeMap();

public:
    /*
     * @brief getCurrentMotionMode 获取当前运动模式
     * 
     * @return int 当前运动模式：-1=未设置, 0=正解模式(关节空间), 1=逆解模式(笛卡尔空间)
     */
    int getCurrentMotionMode() const { return m_currentMotionMode; }
    ZMC_HANDLE getHandle() const { return m_handle; }

    /************************** 正逆解模式控制 *******************************/

    /*
     * @brief waitForAxesIdle 等待指定轴停止运动
     * 
     * 等待指定的轴列表停止运动，最多等待指定的超时时间。
     * 
     * @param axes 轴号列表
     * @param timeoutMs 超时时间（毫秒）
     * @return ZmotionResult 执行结果，success表示是否在超时前所有轴都已停止
     */
    ZmotionResult waitForAxesIdle(const QList<int>& axes, int timeoutMs);

    /*
     * @brief waitForAxesReachTarget 等待轴到达目标位置（对比DPOS，不依赖IDLE）
     *
     * 适用于虚拟轴和CONNFRAME模式，直接对比当前位置与目标位置的偏差。
     *
     * @param axes      轴号列表
     * @param targets   目标位置列表
     * @param timeoutMs 超时时间
     * @param tolerance 位置容差（默认1.0）
     */
    ZmotionResult waitForAxesReachTarget(const QList<int>& axes, const QList<float>& targets,
                                         int timeoutMs, float tolerance = 1.0f);

    /*
     * @brief getIfIdle2 判断轴是否真正停止（引入编码器速度判断）
     *
     * 当 IDLE=-1 且 MSPEED=0 时，轴实际已停止，此函数会正确返回停止状态。
     * 比直接读 IDLE 参数更准确。
     *
     * @param axis 轴号
     * @return ZmotionResult，axisHomeStatus: 0=运动中, 非0=已停止
     */
    ZmotionResult getIfIdle2(int axis);

    /*
     * @brief startInverseKinematics 启动逆解模式（笛卡尔空间控制）
     * 
     * 该函数用于启动机器人的逆运动学模式，将关节轴与虚拟轴建立映射关系，
     * 使得可以通过虚拟轴（笛卡尔坐标）控制机器人运动。
     * 使用ZAux_Direct_Connframe API实现。
     * 
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 切换模式前会等待关节轴停止
     */
    ZmotionResult startInverseKinematics(const QList<int>& jointAxes = {0,1,2,3,4,5},
                                         const QList<int>& virtualAxes = {6,7,8,9,10,11});

    /*
     * @brief startForwardKinematics 启动正解模式（关节空间控制）
     *
     * 该函数用于启动机器人的正运动学模式，将虚拟轴与关节轴建立映射关系，
     * 使得可以通过关节轴控制机器人运动。
     * 使用ZAux_Direct_Connreframe API实现。
     *
     * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
     *         errorCode表示错误码，errorMessage表示错误描述
     * @note 切换模式前会等待虚拟轴停止
     */
    ZmotionResult startForwardKinematics(const QList<int>& jointAxes = {0,1,2,3,4,5},
                                         const QList<int>& virtualAxes = {6,7,8,9,10,11});
};

#endif // ZMOTIONAPI_H

