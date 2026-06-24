/*
 * @file ZmotionAPI.cpp
 * @brief 实现 ZmotionAPI 类，用于封装底层机械臂控制 API 的调用。
 *
 * 本文件负责：
 * - 处理错误码映射
 * - 封装运动指令
 * - 提供统一的返回结果结构
 */
#include "ZmotionAPI.h"
#include <QDebug>
#include <QList>
#include <QThread>
#include <QCoreApplication>

// 初始化静态成员变量
QMap<int, QString> ZmotionAPI::errorCodeMap;
bool ZmotionAPI::s_verbose = true;  // 默认输出详细日志

// 条件日志宏，受 s_verbose 控制
#define ZINFO if (!ZmotionAPI::isVerbose()) {} else qInfo()

const int kFrame = 6;                                 // CONNFRAME编号

/*
 * @brief ZmotionAPI 构造函数
 * 
 * @param parent 父对象指针，默认值为nullptr
 */
ZmotionAPI::ZmotionAPI(int axisMaxCount, QObject *parent) : QObject(parent)
{
    m_handle = 0;
    m_initialized = false;
    m_axisMaxCount = axisMaxCount;
    
    // 初始化错误码映射表
    initializeErrorCodeMap();
}

/*
 * @brief ~ZmotionAPI 析构函数
 */
ZmotionAPI::~ZmotionAPI()
{
    if (m_handle != 0) {
        ZAux_Close(m_handle);
        m_handle = 0;
    }
}

/*
 * @brief isConnected 检查控制器是否已连接并初始化
 *
 * @return bool true表示已连接并初始化，false表示未连接
 */
bool ZmotionAPI::isConnected() const
{
    return m_initialized && m_handle != NULL && m_handle != 0;
}

/*
 * @brief getErrorDescription 根据错误码获取错误描述
 *
 * @param errorCode 错误码
 * @return QString 错误描述
 */
QString ZmotionAPI::getErrorDescription(int errorCode)
{
    if (errorCodeMap.contains(errorCode)) {
        return errorCodeMap.value(errorCode);
    } else {
        return QString("未知错误码: %1").arg(errorCode);
    }
}

/*
 * @brief setAtype 设置轴类型
 *
 * @param axis 轴编号
 * @param type 轴类型（0=虚拟轴, 1=旋转轴, 2=直线轴）
 * @return ZmotionResult 执行结果
 */
ZmotionResult ZmotionAPI::setAtype(int axis, int type)
{
    if (!isConnected()) {
        qWarning() << "设置轴类型失败：控制器未连接";
        return ZmotionResult(false, -1, "控制器未连接");
    }

    int ret = ZAux_Direct_SetAtype(m_handle, axis, type);
    if (ret != 0) {
        QString err = getErrorDescription(ret);
        qWarning() << QString("设置轴%1类型为%2失败, 错误码:%3 %4").arg(axis).arg(type).arg(ret).arg(err);
        return ZmotionResult(false, ret, err);
    }
    return ZmotionResult(true, 0, QString("设置轴%1类型为%2成功").arg(axis).arg(type));
}

/*
 * @brief setTable 写入控制器 TABLE 数据
 *
 * @param tabStart TABLE 起始位置
 * @param count   写入数量
 * @param values  浮点数组
 * @return ZmotionResult 执行结果
 */
ZmotionResult ZmotionAPI::setTable(int tabStart, int count, const QVector<float>& values)
{
    if (!isConnected()) {
        qWarning() << "写入 TABLE 失败：控制器未连接";
        return ZmotionResult(false, -1, "控制器未连接");
    }
    if (values.size() < count) {
        qWarning() << QString("写入 TABLE 失败：数据不足，需要%1个，实际%2个").arg(count).arg(values.size());
        return ZmotionResult(false, -2, "TABLE 数据不足");
    }

    // ZAux_Direct_SetTable 需要非 const 的 float*
    QVector<float> mutableValues = values;
    int ret = ZAux_Direct_SetTable(m_handle, tabStart, count, mutableValues.data());
    if (ret != 0) {
        QString err = getErrorDescription(ret);
        qWarning() << QString("写入 TABLE 失败, 错误码:%1 %2").arg(ret).arg(err);
        return ZmotionResult(false, ret, err);
    }
    ZINFO << QString("成功写入 %1 个 TABLE 数据，起始位置 %2").arg(count).arg(tabStart);
    return ZmotionResult(true, 0, "TABLE 写入成功");
}

/*
 * @brief getTable 读取控制器 TABLE 数据
 *
 * @param tabStart TABLE 起始位置
 * @param count   读取数量
 * @param outValues 输出浮点数组
 * @return ZmotionResult 执行结果
 */
ZmotionResult ZmotionAPI::getTable(int tabStart, int count, QVector<float>& outValues)
{
    if (!isConnected()) {
        qWarning() << "读取 TABLE 失败：控制器未连接";
        return ZmotionResult(false, -1, "控制器未连接");
    }

    outValues.resize(count);
    int ret = ZAux_Direct_GetTable(m_handle, tabStart, count, outValues.data());
    if (ret != 0) {
        QString err = getErrorDescription(ret);
        qWarning() << QString("读取 TABLE 失败, 错误码:%1 %2").arg(ret).arg(err);
        return ZmotionResult(false, ret, err);
    }
    ZINFO << QString("成功读取 %1 个 TABLE 数据，起始位置 %2").arg(count).arg(tabStart);
    return ZmotionResult(true, 0, "TABLE 读取成功");
}

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
ZmotionResult ZmotionAPI::startAxisSingleMoveAbs(int axisNumber, float speed, float accel, float decel, float position)
{
    // 先判断机械臂连接是否成功
    if (!isConnected()) {
        qWarning() << "单轴绝对移动失败：控制器未连接";
        return ZmotionResult(false, -1, "控制器未连接");
    }

    //先设置速度，再设置加速度、减速度，最后启移动
    QList<int> axisList = {axisNumber};
    QList<double> speedList = {speed};
    ZmotionResult result = setAxisSpeed(axisList, speedList);
    if (!result.success) {
        return result;
    }
    QList<double> accelList = {accel};
    result = setAxisAccel(axisList, accelList);
    if (!result.success) {
        return result;
    }
    QList<double> decelList = {decel};
    result = setAxisDecel(axisList, decelList); 
    if (!result.success) {
        return result;
    }
    QList<double> positionList = {position};
    result = setAxisSingleMoveAbs(axisList, positionList);
    if (!result.success) {
        return result;
    }
    return result;
}

/*
 * @brief initializeErrorCodeMap 初始化错误码映射表
 * 
 * 该函数从返回值说明.md文件中提取错误码和对应的错误描述，构建错误码映射表。
 * 如果错误码映射表已经初始化过，直接返回。
 */
void ZmotionAPI::initializeErrorCodeMap()
{
    // 如果已经初始化过，直接返回
    if (!errorCodeMap.isEmpty()) {
        return;
    }
    
    // 从返回值说明.md中提取的错误码映射
    // 文件过大
    errorCodeMap.insert(210, "文件过大");
    // 状态错误
    errorCodeMap.insert(212, "状态错误：Resume时为非暂停状态");
    // 文件下载上传出错，丢包
    errorCodeMap.insert(213, "文件下载上传出错，丢包");
    // 下载文件长度校验错误
    errorCodeMap.insert(214, "下载文件长度校验错误");
    // 缓冲长度不够
    errorCodeMap.insert(215, "缓冲长度不够：发送字符串命令过长");
    // 控制器不支持或禁止的功能
    errorCodeMap.insert(217, "控制器不支持或禁止的功能");
    // 调用传递的参数错误
    errorCodeMap.insert(218, "调用传递的参数错误");
    // 下载冲突
    errorCodeMap.insert(219, "下载冲突，同时启动了多个文件下载");
    // 文件名错误，有特殊字符
    errorCodeMap.insert(220, "文件名错误，有特殊字符");
    // 文件名错误，超过长度
    errorCodeMap.insert(221, "文件名错误，超过长度");
    // 文件不存在
    errorCodeMap.insert(222, "文件不存在");
    // 密码保护限制
    errorCodeMap.insert(223, "密码保护限制");
    errorCodeMap.insert(224, "密码保护限制 2");

    // 硬件错误
    errorCodeMap.insert(260, "硬件错误");
    errorCodeMap.insert(261, "磁盘没有格式化");
    errorCodeMap.insert(262, "RTC错误");
    errorCodeMap.insert(263, "NORFLASH错误");
    errorCodeMap.insert(264, "RAM错误");
    errorCodeMap.insert(265, "NANDFLASH错误");
    errorCodeMap.insert(266, "U盘错误");
    errorCodeMap.insert(267, "FPGA错误");
    errorCodeMap.insert(268, "以太网硬件错误");
    errorCodeMap.insert(271, "备份电源错误");
    errorCodeMap.insert(272, "子卡不存在");
    errorCodeMap.insert(273, "文件丢失");
    errorCodeMap.insert(274, "系统文件错误");
    errorCodeMap.insert(275, "无主控，子卡上产生");
    errorCodeMap.insert(276, "程序文件校验错误");
    errorCodeMap.insert(277, "程序文件错误导致不启动");
    errorCodeMap.insert(278, "ZAR校验 APPPASS出错");
    errorCodeMap.insert(279, "ZAR校验 ID出错");
    errorCodeMap.insert(280, "BAS文件超过最大数量");
    errorCodeMap.insert(281, "子卡 ID冲突，或多主冲突");
    errorCodeMap.insert(282, "不支持的功能");
    errorCodeMap.insert(284, "zar与控制器不匹配");
    errorCodeMap.insert(285, "图片文件错误");
    errorCodeMap.insert(286, "字体文件错误");
    errorCodeMap.insert(288, "以上异常，导致下次开机报错");
    
    // 运动模块错误
    errorCodeMap.insert(1000, "运动模块返回错误偏移");
    errorCodeMap.insert(1002, "无运动缓冲");
    errorCodeMap.insert(1004, "从轴运动中");
    errorCodeMap.insert(1005, "不支持的运动功能");
    errorCodeMap.insert(1006, "圆弧位置错误");
    errorCodeMap.insert(1007, "椭圆 AB参数错误");
    errorCodeMap.insert(1008, "运动模块输入参数错误");
    errorCodeMap.insert(1009, "运动中，无法操作");
    errorCodeMap.insert(1010, "暂停等重复操作");
    errorCodeMap.insert(1011, "IDLE无法做暂停等操作");
    errorCodeMap.insert(1012, "当前运动不支持暂停");
    errorCodeMap.insert(1013, "找不到暂停点");
    errorCodeMap.insert(1014, "ATYPE不支持");
    errorCodeMap.insert(1015, "ZCAN的 ATYPE冲突");
    errorCodeMap.insert(1016, "轴不支持的功能");
    errorCodeMap.insert(1017, "FRAME校正数据错误");
    errorCodeMap.insert(1018, "FRAME校正数据过少");
    errorCodeMap.insert(1019, "FRAME校正数据满足条件的数据过少");
    errorCodeMap.insert(1020, "FRAME校正数据辅助参数过少");
    errorCodeMap.insert(1021, "FRAME校正数据间隔过小，小于关节轴数");
    errorCodeMap.insert(1022, "FRAME的输入坐标错误");
    errorCodeMap.insert(1023, "FRAME状态下坐标不能强制修改");
    errorCodeMap.insert(1024, "FRAME逆解异常");
    errorCodeMap.insert(1025, "不是 FRAME状态");
    errorCodeMap.insert(1026, "FRAME HAND错误");
    errorCodeMap.insert(1027, "姿态在插补中不能切换");
    errorCodeMap.insert(1028, "特殊关节轴与虚拟轴当量要求一样");
    errorCodeMap.insert(1030, "CORNERMODE 7位设置了但不支持此运动");
    errorCodeMap.insert(1031, "CORNERMODE 7位设置了但不是 FRAME状态");
    errorCodeMap.insert(1032, "AXIS_ADDRESS错误");
    
    // ZBASIC模块错误
    errorCodeMap.insert(2000, "ZBASIC模块偏移");
    errorCodeMap.insert(2021, "手动停止");
    errorCodeMap.insert(2022, "因其他任务错误导致本任务停止");
    errorCodeMap.insert(2023, "试图修改只读状态参数");
    errorCodeMap.insert(2024, "数组越界");
    errorCodeMap.insert(2025, "变量数超过控制器规格");
    errorCodeMap.insert(2026, "数组数超过控制器规格");
    errorCodeMap.insert(2027, "数组空间超过控制器规格");
    errorCodeMap.insert(2028, "SUB数超过控制器规格");
    errorCodeMap.insert(2029, "标识符命名错误");
    errorCodeMap.insert(2030, "标识符命名过长");
    errorCodeMap.insert(2031, "没有右括号");
    errorCodeMap.insert(2032, "认识不到的字符");
    errorCodeMap.insert(2033, "表达式中碰到认识不到的名称");
    errorCodeMap.insert(2034, "SUB不能在表达式中使用");
    errorCodeMap.insert(2043, "认识不到的命令标识符，当前行第一个标识名称");
    errorCodeMap.insert(2044, "堆栈溢出");
    errorCodeMap.insert(2045, "数学表达式太复杂，不同控制器的规格不一样");
    errorCodeMap.insert(2046, "没有找到结束引用标号");
    errorCodeMap.insert(2047, "指令没有返回值，不能用于表达式计算");
    errorCodeMap.insert(2048, "函数必须返回值，不在一行的开头地方");
    errorCodeMap.insert(2049, "特殊指令必须单独一行");
    errorCodeMap.insert(2050, "参数或数组需要索引");
    errorCodeMap.insert(2051, "变量不能使用索引");
    errorCodeMap.insert(2052, "数组重定义且长度不一致");
    errorCodeMap.insert(2053, "数组定义长度参数错误，负数或过大");
    errorCodeMap.insert(2054, "标识符已经定义为 SUB 过程，不能再做其他用途");
    errorCodeMap.insert(2055, "标识符已经定义为参数，不能再做其他用途");
    errorCodeMap.insert(2056, "标识符预留，不能使用");
    errorCodeMap.insert(2057, "出现不能识别的字符");
    errorCodeMap.insert(2058, "SUB 调用重复出栈");
    errorCodeMap.insert(2060, "语法格式错误");
    errorCodeMap.insert(2062, "函数参数范围错误，包括任务号超过范围");
    errorCodeMap.insert(2063, "函数参数过多");
    errorCodeMap.insert(2064, "函数参数太少");
    errorCodeMap.insert(2065, "缺少操作数");
    errorCodeMap.insert(2066, "操作符后面缺少操作数");
    errorCodeMap.insert(2067, "操作符前面缺少操作数");
    errorCodeMap.insert(2068, "不认识的操作符");
    errorCodeMap.insert(2069, "需要双目操作符");
    errorCodeMap.insert(2070, "CALL 必须调用 SUB");
    errorCodeMap.insert(2072, "需要赋值符号");
    errorCodeMap.insert(2073, "空文件");
    errorCodeMap.insert(2074, "SUB 定义的标识符名称冲突");
    errorCodeMap.insert(2075, "要启动的任务已经运行中");
    errorCodeMap.insert(2076, "多个参数要使用逗号隔开");
    errorCodeMap.insert(2077, "括号不配对，无左括号");
    errorCodeMap.insert(2078, "IF 判断的嵌套太多");
    errorCodeMap.insert(2079, "循环语句嵌套太多");
    errorCodeMap.insert(2080, "插补轴数太少");
    errorCodeMap.insert(2081, "CONST 常量，不能修改");
    errorCodeMap.insert(2082, "命令不能从 PC 在线发送");
    errorCodeMap.insert(2083, "SUB 定义的参数太多");
    errorCodeMap.insert(2084, "SUB 带参数，不能用于 GOTO 语句");
    errorCodeMap.insert(2085, "局部标识符定义太多");
    errorCodeMap.insert(2086, "LOCAL变量名与文件变量名或其它标识符发生冲突");
    errorCodeMap.insert(2087, "LOCAL不支持数组定义");
    errorCodeMap.insert(2088, "GSUB定义的参数字母重复");
    errorCodeMap.insert(2089, "GSUB定义的参数只能为单字母");
    errorCodeMap.insert(2090, "不能修改只读参数");
    errorCodeMap.insert(2091, "GSUB_IFPARA函数使用场合错误");
    errorCodeMap.insert(2092, "除数为零");
    errorCodeMap.insert(2093, "超过缓冲区大小限制");
    errorCodeMap.insert(2094, "在线命令阻塞时间过长");
    errorCodeMap.insert(2095, "参数重命名");
    errorCodeMap.insert(2096, "值未初始化就被使用了");
    errorCodeMap.insert(2097, "轴号冲突");
    errorCodeMap.insert(2099, "内部错误");
    errorCodeMap.insert(2100, "SCANEDGE个数过多");
    errorCodeMap.insert(2101, "ZINDEX类型不匹配");
    
    // 系统错误
    errorCodeMap.insert(2901, "系统错误：定义的标识符数量过多");
    
    // 缓冲区错误
    errorCodeMap.insert(3201, "超过缓冲区大小限制");
    errorCodeMap.insert(3202, "文件非正常结束");
    errorCodeMap.insert(3204, "内部状态错误");
    errorCodeMap.insert(3205, "不支持的功能被调用了");
    errorCodeMap.insert(3206, "内部调用参数错误");
    errorCodeMap.insert(3231, "资源不足");
    errorCodeMap.insert(3242, "OS错误");
    errorCodeMap.insert(3243, "U盘未插好");
    errorCodeMap.insert(3244, "文件已被其他程序占用");
    errorCodeMap.insert(3245, "文件太大无法处理");
    errorCodeMap.insert(3248, "文件名格式错误");
    errorCodeMap.insert(3249, "文件名太长无法识别");
    errorCodeMap.insert(3250, "找不到指定文件");
    
    // 圆弧错误
    errorCodeMap.insert(3301, "圆弧上的三个点共线");
    errorCodeMap.insert(3302, "相交于同一点的两条直线无法生成圆弧");
    
    // Modbus错误
    errorCodeMap.insert(3401, "MODBUS主端参数错误：总长度超出规定范围");
    errorCodeMap.insert(3402, "消息响应超时");
    errorCodeMap.insert(3407, "Modbus返回参数错误");
    errorCodeMap.insert(3408, "Modbus返回不支持");
    errorCodeMap.insert(3421, "Modbus从端返回不支持的功能码");
    errorCodeMap.insert(3422, "Modbus从端返回地址空间错误");
    errorCodeMap.insert(3423, "Modbus从端返回数据长度不对");
    errorCodeMap.insert(3424, "Modbus从端返回长度过长");
    
    // ZCAN错误
    errorCodeMap.insert(3501, "ZCAN返回无子卡");
    errorCodeMap.insert(3502, "ZCAN返回子卡无对应轴");
    
    // PLC模块错误
    errorCodeMap.insert(4000, "PLC模块的错误（4000-4500）");
    errorCodeMap.insert(4002, "参数错误");
    errorCodeMap.insert(4003, "未知类型");
    errorCodeMap.insert(4004, "未知函数");
    errorCodeMap.insert(4005, "压栈太多 STL");
    errorCodeMap.insert(4006, "压栈太多");
    errorCodeMap.insert(4007, "程序太复杂，BLOCK太多");
    errorCodeMap.insert(4008, "没有压栈 BLOCK");
    errorCodeMap.insert(4009, "没有压栈 STL");
    errorCodeMap.insert(4010, "没有压栈");
    errorCodeMap.insert(4014, "文件内容错误");
    errorCodeMap.insert(4015, "RET必须在STL的后面");
    errorCodeMap.insert(4016, "超过范围");
    errorCodeMap.insert(4017, "低于范围");
    errorCodeMap.insert(4018, "L没有定义");
    errorCodeMap.insert(4019, "不支持G代码函数");
    errorCodeMap.insert(4020, "不能GOTO跨PLC与BASIC");
    errorCodeMap.insert(4021, "PLC主任务只有一个");
    errorCodeMap.insert(4022, "语法错误");
    errorCodeMap.insert(4023, "FOR NEXT错误，不匹配");
    errorCodeMap.insert(4024, "FOR NEXT错误，无NEXT");
    errorCodeMap.insert(4026, "FOR MC混用");
    errorCodeMap.insert(4027, "FOR STL混用");
    errorCodeMap.insert(4030, "必须在PLC主任务中使用");
    errorCodeMap.insert(4031, "必须在中断中使用");
    errorCodeMap.insert(4032, "参数个数少");
    errorCodeMap.insert(4033, "参数个数多");
    errorCodeMap.insert(4034, "要8的倍数");
    errorCodeMap.insert(4035, "寄存器标识错误");
    errorCodeMap.insert(4036, "寄存器类型错误");
    errorCodeMap.insert(4037, "LV个数超过");
    errorCodeMap.insert(4038, "只读");
    
    // PLC上位机端错误
    errorCodeMap.insert(4500, "PLC上位机端错误（4500-5000）");
    errorCodeMap.insert(4503, "内存不够");
    errorCodeMap.insert(4504, "回流到母线上");
    errorCodeMap.insert(4505, "回流");
    errorCodeMap.insert(4506, "AND类型不能直接接母线");
    errorCodeMap.insert(4510, "悬空");
    errorCodeMap.insert(4511, "最右端必须是输出类型");
    
    // HMI模块错误
    errorCodeMap.insert(5000, "HMI模块的错误（5000-5500）");
    errorCodeMap.insert(5002, "LCD号冲突");
    errorCodeMap.insert(5003, "不支持对象");
    errorCodeMap.insert(5004, "内存不够");
    errorCodeMap.insert(5005, "控件层次错误");
    errorCodeMap.insert(5006, "窗口号超过");
    errorCodeMap.insert(5007, "无效窗口号");
    errorCodeMap.insert(5010, "对象属性丢失");
    errorCodeMap.insert(5011, "输入窗口有多个显示元件");
    errorCodeMap.insert(5012, "ACTION类型错误");
    errorCodeMap.insert(5013, "事件过多");
    errorCodeMap.insert(5014, "返回上个窗口失败");
    errorCodeMap.insert(5015, "不能关闭基本窗口");
    errorCodeMap.insert(5016, "字体中找不到对应字符");
    errorCodeMap.insert(5017, "必须在 HMI任务中使用");
    errorCodeMap.insert(5020, "控件 ID冲突");
    errorCodeMap.insert(5021, "LCD号错误");
    errorCodeMap.insert(5022, "找不到可用 LCD");
    errorCodeMap.insert(5023, "LCD没有打开");
    errorCodeMap.insert(5024, "LCD无数据");
    errorCodeMap.insert(5025, "程序复位");
    errorCodeMap.insert(5026, "lcd已经打开了");
    errorCodeMap.insert(5027, "不是网络 LCD");
    errorCodeMap.insert(5028, "不支持的压缩方式");
    errorCodeMap.insert(5029, "颜色深度不支持");
    errorCodeMap.insert(5030, "不支持的数据类型");
    errorCodeMap.insert(5031, "设备号错误");
    errorCodeMap.insert(5032, "LCD_SEL不能使用");
    errorCodeMap.insert(5033, "设置 REDRAW不能再 DRAW阶段");
    errorCodeMap.insert(5034, "DRAW函数只能在 DRAW阶段");
    errorCodeMap.insert(5035, "操作不能再 DRAW阶段调用");
    errorCodeMap.insert(5036, "内部 LCD分辨率固定");
    errorCodeMap.insert(5037, "LCD分辨率超过");
    errorCodeMap.insert(5038, "库文件名错误");
    errorCodeMap.insert(5039, "字符过多");
    
    // PC端 PLC文件编译的错误
    errorCodeMap.insert(5503, "内存不够");
    errorCodeMap.insert(5504, "回流到母线上");
    errorCodeMap.insert(5505, "回流");
    errorCodeMap.insert(5506, "AND类型指令不能直接接母线");
    errorCodeMap.insert(5510, "右边悬空，没有接输出指令");
    errorCodeMap.insert(5511, "最右边不是输出类型指令");
    errorCodeMap.insert(5512, "最右边不能连接在一起");
    errorCodeMap.insert(5513, "输出类型指令必须在最右边");
    errorCodeMap.insert(5514, "不支持的指令类型");
    errorCodeMap.insert(5517, "寄存器没有值");
    errorCodeMap.insert(5518, "DOT值超过范围");
    errorCodeMap.insert(5519, "索引寄存器超过范围");
    errorCodeMap.insert(5520, "字符数过多");
    errorCodeMap.insert(5521, "寄存器类型错误");
    errorCodeMap.insert(5522, "寄存器值错误");
    errorCodeMap.insert(5523, "寄存器个数过多");
    errorCodeMap.insert(5524, "寄存器个数过少");
    errorCodeMap.insert(5525, "STL使用错误");
    errorCodeMap.insert(5526, "RET使用错误");
    errorCodeMap.insert(5527, "重复 RET");
    errorCodeMap.insert(5528, "END或 LBL 的位置错误");
    errorCodeMap.insert(5529, "函数不能直接接母线");
    errorCodeMap.insert(5530, "出栈没有压栈");
    errorCodeMap.insert(5531, "MPP 太多");
    errorCodeMap.insert(5532, "寄存器类型使用错误");
    errorCodeMap.insert(5533, "ANB错误，块数不够");
    errorCodeMap.insert(5534, "ORB错误，块数不够");
    errorCodeMap.insert(5535, "ANB错误，输出操作后不能合并");
    errorCodeMap.insert(5536, "ORB错误，输出操作后不能合并");
    errorCodeMap.insert(5537, "AND直接接母线");
    errorCodeMap.insert(5538, "OR直接接母线");
    errorCodeMap.insert(5539, "OR不能在 OUT指令的后面");
    errorCodeMap.insert(5540, "STL和 MC不能共用");
    errorCodeMap.insert(5541, "MC不能直接接母线");
    errorCodeMap.insert(5542, "_@寄存器要括号");
    errorCodeMap.insert(5543, "注释错误");
    errorCodeMap.insert(5544, "梯形图列数过多");
    errorCodeMap.insert(5545, "输出类型不能直接接母线");
    
    // ECAT模块错误
    errorCodeMap.insert(6000, "ECAT模块错误，slot编号错误");
    errorCodeMap.insert(6001, "内部错误，功能不支持");
    errorCodeMap.insert(6005, "参数错误");
    errorCodeMap.insert(6006, "支持的设备类型数超过限制");
    errorCodeMap.insert(6009, "操作 node超过从个数");
    errorCodeMap.insert(6012, "资源不够");
    errorCodeMap.insert(6013, "从设备反应超时");
    errorCodeMap.insert(6014, "缓冲不够");
    errorCodeMap.insert(6015, "应答包 wkc错误");
    errorCodeMap.insert(6016, "SDO应答超长");
    errorCodeMap.insert(6017, "SDO应答错误");
    errorCodeMap.insert(6018, "SDO应答数据长度错误");
    errorCodeMap.insert(6019, "WKC超时");
    errorCodeMap.insert(6020, "state切换超时");
    errorCodeMap.insert(6021, "SDO ABORT");
    errorCodeMap.insert(6023, "NODE profile错误");
    errorCodeMap.insert(6024, "轴 profile错误");
    errorCodeMap.insert(6025, "轴数超过");
    errorCodeMap.insert(6029, "PDO列表个数超过");
    errorCodeMap.insert(6031, "设备个数超过");
    errorCodeMap.insert(6042, "设备不支持");
    errorCodeMap.insert(6045, "邮箱超时");
    errorCodeMap.insert(6047, "数据类型错误");
    errorCodeMap.insert(6049, "模块不支持的子模块");
    errorCodeMap.insert(6050, "模块子模块数量超过");
    errorCodeMap.insert(6051, "模块不认识的子模块");
    
    // RTEX驱动错误
    errorCodeMap.insert(6208, "RTEX驱动 ID 冲突");
    errorCodeMap.insert(6209, "扫描超时，一般为网线接触问题");
    errorCodeMap.insert(6210, "RTEX 初始化失败");
    errorCodeMap.insert(6211, "RTEX 扫描结果错误");
    
    // PC 端产生错误
    errorCodeMap.insert(20000, "PC 端产生错误的偏移");
    errorCodeMap.insert(20002, "参数错");
    errorCodeMap.insert(20003, "超时（可能是 FIFO 缓冲阻塞）");
    errorCodeMap.insert(20006, "操作系统错误");
    errorCodeMap.insert(20007, "串口打开失败");
    errorCodeMap.insert(20008, "网络打开失败");
    errorCodeMap.insert(20009, "句柄错误");
    errorCodeMap.insert(20010, "发送错误");
    errorCodeMap.insert(20011, "文件错误");
    errorCodeMap.insert(20012, "文件长度错误");
    errorCodeMap.insert(20013, "文件名过长");
    errorCodeMap.insert(20014, "文件不存在");
    errorCodeMap.insert(20015, "ZLB 库文件错误");
    errorCodeMap.insert(20016, "文件没有编译（一般 PLC 文件）");
    errorCodeMap.insert(20020, "固件文件不匹配");
    errorCodeMap.insert(20021, "不支持的功能");
    errorCodeMap.insert(20030, "输入缓冲长度不够");
    errorCodeMap.insert(20100, "应答缓冲长度不够");
}

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
ZmotionResult ZmotionAPI::searchEthlist()
{
    try {
        // 分配足够大的缓冲区来存储IP列表
        const int bufferSize = 10240;
        char* ipBuffer = new char[bufferSize];
        
        // 调用ZAux_SearchEthlist搜索网络中的控制器
        int result = ZAux_SearchEthlist(ipBuffer, bufferSize - 10, 200);
        
        // 创建返回结果结构体
        ZmotionResult zResult;
        
        if (result == 0) {
            // 搜索成功，解析IP列表
            QString ipListStr(ipBuffer);
            QStringList ipList = ipListStr.split(' ', Qt::SkipEmptyParts);
            zResult.ipList = ipList;
            zResult.success = true;
            zResult.errorCode = 0;
            ZINFO << "搜索到" << ipList.size() << "个ZMotion控制器";
        } else {
            // 搜索失败
            zResult.success = false;
            zResult.errorCode = result;
            zResult.errorMessage = getErrorDescription(result);
            qWarning() << "搜索ZMotion控制器失败，错误码:" << result << "，错误描述:" << zResult.errorMessage;
        }
        
        // 释放缓冲区内存
        delete[] ipBuffer;
        
        return zResult;
    } catch (const std::exception& e) {
        // 捕获异常
        qCritical() << "搜索ZMotion控制器时发生异常:" << e.what();
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    } catch (...) {
        // 捕获未知异常
        qCritical() << "搜索ZMotion控制器时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/*
 * @brief close 关闭控制器连接
 * 
 * 该函数用于关闭当前与ZMotion控制器的连接。
 * 
 * @return ZmotionResult 包含关闭结果的结构体，success表示是否成功，
 *         errorCode表示错误码，errorMessage表示错误描述
 */
ZmotionResult ZmotionAPI::close()
{
    try {
        // 如果未初始化，返回成功
        if (!m_initialized || m_handle == NULL) {
            return ZmotionResult(true, 0, "未连接到控制器或连接已关闭");
        }
        
        // 调用ZAux_Close关闭连接
        int result = ZAux_Close(m_handle);
        
        // 创建返回结果结构体
        ZmotionResult zResult;
        
        if (result == 0) {
            // 关闭成功
            zResult.success = true;
            zResult.errorCode = 0;
            zResult.errorMessage = "成功关闭控制器连接";
            m_initialized = false;
            m_handle = NULL;
            ZINFO << "成功关闭ZMotion控制器连接";
        } else {
            // 关闭失败
            zResult.success = false;
            zResult.errorCode = result;
            zResult.errorMessage = getErrorDescription(result);
            m_initialized = false; // 即使关闭失败，也将状态设为未初始化
            m_handle = NULL;
            qWarning() << "关闭ZMotion控制器连接失败，错误码:" << result 
                      << "，错误描述:" << zResult.errorMessage;
        }
        
        return zResult;
    } catch (const std::exception& e) {
        // 捕获异常
        qCritical() << "关闭ZMotion控制器连接时发生异常:" << e.what();
        m_initialized = false;
        m_handle = NULL;
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    } catch (...) {
        // 捕获未知异常
        qCritical() << "关闭ZMotion控制器连接时发生未知异常";
        m_initialized = false;
        m_handle = NULL;
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::getAxisCount()
{
    try {
        // 如果未初始化，返回失败
        if (!m_initialized || m_handle == NULL) {
            return ZmotionResult(false, -1, "未连接到控制器", QStringList(), QList<int>(), 0, 0.0f, 0);
        }

        // 调用 ZMC_GetAxises 获取轴数
        uint8 axisCount = ZMC_GetAxises(m_handle);

        // 创建返回结果结构体
        ZmotionResult zResult;
        zResult.success = true;
        zResult.errorCode = 0;
        zResult.errorMessage = "成功获取控制器轴数量";
        zResult.axisCount = static_cast<int>(axisCount);

        ZINFO << "成功获取控制器轴数量:" << axisCount;
        return zResult;
    } catch (const std::exception& e) {
        // 捕获异常
        qCritical() << "获取控制器轴数量时发生异常:" << e.what();
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    } catch (...) {
        // 捕获未知异常
        qCritical() << "获取控制器轴数量时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::openEth(const QString& ipaddr)
{
    try {
        // 如果已经初始化，先关闭当前连接
        if (m_initialized) {
            ZAux_Close(m_handle);
            m_initialized = false;
        }
        
        // 调用ZAux_OpenEth连接控制器
        int result = ZAux_OpenEth(const_cast<char*>(ipaddr.toLocal8Bit().constData()), &m_handle);
        
        // 创建返回结果结构体
        ZmotionResult zResult;
        
        if (result == 0) {
            // 连接成功
            zResult.success = true;
            zResult.errorCode = 0;
            zResult.errorMessage = QString("成功连接到控制器 %1").arg(ipaddr);
            m_initialized = true;
            ZINFO << "成功连接到ZMotion控制器:" << ipaddr;
        } else {
            // 连接失败
            zResult.success = false;
            zResult.errorCode = result;
            zResult.errorMessage = getErrorDescription(result);
            m_initialized = false;
            qWarning() << "连接ZMotion控制器失败，IP:" << ipaddr << "，错误码:" << result 
                      << "，错误描述:" << zResult.errorMessage;
        }
        
        return zResult;
    } catch (const std::exception& e) {
        // 捕获异常
        qCritical() << "连接ZMotion控制器时发生异常:" << e.what();
        m_initialized = false;
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    } catch (...) {
        // 捕获未知异常
        qCritical() << "连接ZMotion控制器时发生未知异常";
        m_initialized = false;
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/*
 * @brief openCom 串口连接控制器
 */
ZmotionResult ZmotionAPI::openCom(uint32 comPort, uint32 baudRate,
                                  uint32 dataBits, uint32 parity, uint32 stopBits)
{
    try {
        if (m_initialized) {
            ZAux_Close(m_handle);
            m_initialized = false;
        }

        // 设置串口通讯参数
        int ret = ZAux_SetComDefaultBaud(baudRate, dataBits, parity, stopBits);
        if (ret != 0) {
            qWarning() << "[ZMotion] 设置串口参数失败, 错误码:" << ret;
            return ZmotionResult(false, ret, getErrorDescription(ret));
        }

        // 打开串口连接
        ret = ZAux_OpenCom(comPort, &m_handle);

        ZmotionResult zResult;
        if (ret == 0) {
            zResult.success = true;
            zResult.errorCode = 0;
            zResult.errorMessage = QString("串口 COM%1 连接成功 (%2 baud)")
                                      .arg(comPort).arg(baudRate);
            m_initialized = true;
            ZINFO << zResult.errorMessage;
        } else {
            zResult.success = false;
            zResult.errorCode = ret;
            zResult.errorMessage = QString("串口 COM%1 连接失败: %2")
                                      .arg(comPort).arg(getErrorDescription(ret));
            m_initialized = false;
            qWarning() << zResult.errorMessage;
        }
        return zResult;
    } catch (const std::exception& e) {
        m_initialized = false;
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    } catch (...) {
        m_initialized = false;
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/*
 * @brief executeCommand 发送 ZBasic 在线命令
 */
ZmotionResult ZmotionAPI::executeCommand(const QString& command, QString* response)
{
    if (!m_initialized || !m_handle) {
        return ZmotionResult(false, -1, "控制器未连接");
    }

    char buf[4096] = {0};
    uint32 readLen = 0;

    // 先尝试 ZAux_DirectCommand（直接方式，立即执行）
    int ret = ZAux_DirectCommand(m_handle, command.toLocal8Bit().constData(), buf, sizeof(buf));
    qWarning() << "[ZMotion] DirectCommand:" << command << "ret=" << ret << "response=" << buf;

    if (ret != 0) {
        // DirectCommand 失败，尝试 ZAux_Execute（缓冲方式）
        memset(buf, 0, sizeof(buf));
        ret = ZAux_Execute(m_handle, command.toLocal8Bit().constData(), buf, sizeof(buf));
        qWarning() << "[ZMotion] Execute:" << command << "ret=" << ret << "response=" << buf;
    }

    ZmotionResult zResult;
    zResult.errorCode = ret;

    if (ret == 0) {
        zResult.success = true;
        zResult.errorMessage = QString("命令执行成功: %1").arg(command);
        if (response) {
            *response = QString::fromLocal8Bit(buf).trimmed();
            if (response->isEmpty()) {
                *response = QStringLiteral("(空响应，控制器可能未返回数据)");
            }
        }
    } else {
        zResult.success = false;
        zResult.errorMessage = QString("命令失败: %1 (错误码=%2)").arg(command).arg(ret);
        qWarning() << "[ZMotion]" << zResult.errorMessage;
    }
    return zResult;
}

/*
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
 */
ZmotionResult ZmotionAPI::singleAxisCancel(const QList<int>& axes, int mode) {
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "单轴停止失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 检查轴号列表是否为空
        if (axes.isEmpty()) {
            qWarning() << "单轴停止失败：轴号列表为空";
            return ZmotionResult(false, -1, "轴号列表为空");
        }
        
        // 检查停止模式是否在有效范围内
        if (mode < 0 || mode > 3) {
            qWarning() << "单轴停止失败：无效的停止模式";
            return ZmotionResult(false, -1, "无效的停止模式，支持的模式为0-3");
        }
        
        // 跟踪成功和失败的轴号
        QList<int> successAxes;
        QList<int> failedAxes;
        QMap<int, int> errorCodes;
        
        // 遍历每个轴号执行停止操作
        for (int axis : axes) {
            // 验证轴号的有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("单轴停止失败：轴号 %1 超出有效范围(0-%2)")
                             .arg(axis).arg(m_axisMaxCount - 1);
                failedAxes.append(axis);
                errorCodes[axis] = -2;
                continue;
            }
            
            // 调用ZMotion API执行单轴停止
            int result = ZAux_Direct_Single_Cancel(m_handle, axis, mode);
            
            if (result == 0) {
                // 成功停止
                successAxes.append(axis);
                ZINFO << QString("轴 %1 停止成功，模式 %2")
                        .arg(axis).arg(mode);
            } else {
                // 停止失败
                failedAxes.append(axis);
                errorCodes[axis] = result;
                qWarning() << QString("轴 %1 停止失败，错误码：%2")
                             .arg(axis).arg(result);
            }
        }
        
        // 处理部分失败的情况
        if (!failedAxes.isEmpty()) {
            // 将int列表转换为字符串列表
            QStringList successAxesStr;
            for (int axis : successAxes) {
                successAxesStr << QString::number(axis);
            }
            
            QStringList failedAxesStr;
            for (int axis : failedAxes) {
                failedAxesStr << QString::number(axis);
            }
            
            QString errorMsg = QString("部分轴停止失败。成功轴：%1，失败轴：%2。")
                              .arg(successAxesStr.join(","))
                              .arg(failedAxesStr.join(","));
            
            // 添加第一个失败轴的错误描述
            int firstFailedAxis = failedAxes.first();
            int errorCode = errorCodes[firstFailedAxis];
            errorMsg += QString("轴 %1 错误描述：%2")
                       .arg(firstFailedAxis).arg(getErrorDescription(errorCode));
            
            return ZmotionResult(false, errorCode, errorMsg);
        }
        
        // 全部成功
        // 将int列表转换为字符串列表
        QStringList axesStr;
        for (int axis : axes) {
            axesStr << QString::number(axis);
        }
        
        return ZmotionResult(true, 0, QString("轴 %1 停止成功，模式 %2")
                           .arg(axesStr.join(",")).arg(mode));
        
    } catch (const std::exception& e) {
        qCritical() << QString("单轴停止时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(e.what()));
    } catch (...) {
        qCritical() << "单轴停止时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
 */
ZmotionResult ZmotionAPI::rapidStop(int mode) {
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "所有轴立即停止失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 检查停止模式是否在有效范围内
        if (mode < 0 || mode > 3) {
            qWarning() << "所有轴立即停止失败：无效的停止模式";
            return ZmotionResult(false, -1, "无效的停止模式，支持的模式为0-3");
        }
        
        // 调用ZMotion API执行所有轴立即停止
        int result = ZAux_Direct_Rapidstop(m_handle, mode);
        
        if (result == 0) {
            // 成功停止所有轴
            ZINFO << QString("所有轴立即停止成功，模式 %1").arg(mode);
            return ZmotionResult(true, 0, QString("所有轴立即停止成功，模式 %1").arg(mode));
        } else {
            // 停止失败
            qWarning() << QString("所有轴立即停止失败，错误码：%1").arg(result);
            return ZmotionResult(false, result, 
                               QString("所有轴立即停止失败：%1").arg(getErrorDescription(result)));
        }
        
    } catch (const std::exception& e) {
        qCritical() << QString("所有轴立即停止时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(e.what()));
    } catch (...) {
        qCritical() << "所有轴立即停止时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/*
 * @brief clearAxisPosition 轴位置清零和编码器清零
 * 
 * 该函数用于将指定轴的位置和编码器同时清零。
 * 使用ZAux_Direct_SetDpos设置指令位置为0，使用ZAux_Direct_SetMpos设置实际位置为0。
 * 
 * @param axes 轴号列表（0开始），支持单个或多个轴号
 * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
 *         errorCode表示错误码，errorMessage表示错误描述
 */
ZmotionResult ZmotionAPI::clearAxisPosition(const QList<int>& axes)
{
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "清除轴位置失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 检查轴号列表是否为空
        if (axes.isEmpty()) {
            qWarning() << "清除轴位置失败：轴号列表为空";
            return ZmotionResult(false, -2, "轴号列表为空");
        }
        
        // 验证所有轴号的有效性
        for (int axis : axes) {
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("清除轴位置失败：轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis);
                return ZmotionResult(false, -3, QString("轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis));
            }
        }
        
        // 遍历所有轴，执行清零操作
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        for (int axis : axes) {
            // 先设置指令位置为0
            int dposResult = ZAux_Direct_SetDpos(m_handle, axis, 0);
            if (dposResult != 0) {
                QString errorDesc = getErrorDescription(dposResult);
                qWarning() << QString("设置轴%1指令位置为0失败，错误码：%2，错误描述：%3").arg(axis).arg(dposResult).arg(errorDesc);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = dposResult;
                    firstErrorMessage = errorDesc;
                }
                continue; // 跳过当前轴，继续处理下一个轴
            }
            
            // 再设置实际位置为0
            int mposResult = ZAux_Direct_SetMpos(m_handle, axis, 0);
            if (mposResult != 0) {
                QString errorDesc = getErrorDescription(mposResult);
                qWarning() << QString("设置轴%1实际位置为0失败，错误码：%2，错误描述：%3").arg(axis).arg(mposResult).arg(errorDesc);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = mposResult;
                    firstErrorMessage = errorDesc;
                }
                continue; // 跳过当前轴，继续处理下一个轴
            }
            
            // 成功完成清零操作
            successAxes << QString::number(axis);
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功清零
            ZINFO << QString("成功将轴%1的位置和编码器清零").arg(successAxes.join(","));
            return ZmotionResult(true, 0, QString("成功将轴%1的位置和编码器清零").arg(successAxes.join(",")));
        } else {
            // 部分轴清零失败
            qWarning() << QString("部分轴位置清零失败。成功轴：%1，失败轴：%2").arg(successAxes.join(",")).arg(failedAxes.join(","));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴位置清零失败。成功轴：%1，失败轴：%2，第一个错误：%3").
                               arg(successAxes.join(",")).arg(failedAxes.join(",")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("清除轴位置时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "清除轴位置时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/**
 * @brief setAxisDpos 设置轴的指令位置
 *
 * 仅修改控制器内部 DPOS，不产生物理运动。
 * 使用 ZAux_Direct_SetDpos 实现。
 */
ZmotionResult ZmotionAPI::setAxisDpos(const QList<int>& axes, const QList<double>& positions)
{
    try {
        if (!m_initialized || m_handle == NULL) {
            return ZmotionResult(false, -1, "控制器未连接");
        }
        if (axes.size() != positions.size()) {
            return ZmotionResult(false, -2, "轴号列表与位置列表长度不匹配");
        }

        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            float pos = static_cast<float>(positions.at(i));
            int ret = ZAux_Direct_SetDpos(m_handle, axis, pos);
            if (ret != 0) {
                return ZmotionResult(false, ret, getErrorDescription(ret));
            }
        }
        return ZmotionResult(true, 0, "DPOS设置成功");
    }
    catch (const std::exception& e) {
        return ZmotionResult(false, -1, QString("异常: ") + e.what());
    }
    catch (...) {
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::getAxisPosition(int axis)
{
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "获取单轴位置失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 验证轴号的有效性
        // 支持读取虚拟轴位置（轴号 0-11），虚拟轴由 CONNFRAME 创建
        if (axis < 0 || axis > 11) {
            qWarning() << QString("获取单轴位置失败：轴号无效，轴号范围应为0-11，当前值：%1").arg(axis);
            return ZmotionResult(false, -2, QString("轴号无效，轴号范围应为0-11，当前值：%1").arg(axis));
        }
        
        // 调用底层API获取轴位置
        float position = 0.0;
        int result = ZAux_Direct_GetDpos(m_handle, axis, &position);
        if (result != 0) {
            QString errorMsg = getErrorDescription(result);
            qWarning() << QString("获取单轴位置失败：轴号%1，错误码：%2，错误信息：%3").arg(axis).arg(result).arg(errorMsg);
            return ZmotionResult(false, result, errorMsg);
        }
        
        // 设置返回结果
        ZmotionResult zmResult;
        zmResult.success = true;
        zmResult.axisPosition = position;
        
        // qDebug() << QString("获取单轴位置成功：轴号%1，当前位置：%2").arg(axis).arg(position);
        return zmResult;
    } catch (const std::exception& e) {
        qCritical() << QString("获取单轴位置时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -3, QString("发生异常：%1").arg(e.what()));
    } catch (...) {
        qCritical() << "获取单轴位置时发生未知异常";
        return ZmotionResult(false, -4, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::getAxisParamValue(int axis, AxisParamsCommand paramType)
{
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "获取轴参数值失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 验证轴号的有效性
        if (axis < 0 || axis >= m_axisMaxCount) {
            qWarning() << QString("获取轴参数值失败：轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis);
            return ZmotionResult(false, -2, QString("轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis));
        }
        
        // 调用底层API获取轴参数值
        float paramValue = 0.0;
        int result = ZAux_Direct_GetParam(m_handle, axisParamsCommandToStringMap[paramType].toStdString().c_str(), axis, &paramValue);
        if (result != 0) {
            QString errorMsg = getErrorDescription(result);
            qWarning() << QString("获取轴参数值失败：轴号%1，参数类型%2，错误码：%3，错误信息：%4").arg(axis).arg(axisParamsCommandToStringMap[paramType]).arg(result).arg(errorMsg);
            return ZmotionResult(false, result, errorMsg);
        }
        
        // 设置返回结果
        ZmotionResult zmResult;
        zmResult.success = true;
        zmResult.axisParamValue = paramValue;
        
        // qDebug() << QString("获取轴参数值成功：轴号%1，参数类型%2，当前值：%3").arg(axis).arg(axisParamsCommandToStringMap[paramType]).arg(paramValue);
        return zmResult;
    } catch (const std::exception& e) {
        qCritical() << QString("获取轴参数值时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -3, QString("发生异常：%1").arg(e.what()));
    } catch (...) {
        qCritical() << "获取轴参数值时发生未知异常";
        return ZmotionResult(false, -4, "发生未知异常");
    }   
}

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
ZmotionResult ZmotionAPI::setAxisParamValue(int axis, AxisParamsCommand paramType, float paramValue)
{
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "设置轴参数值失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 验证轴号的有效性
        if (axis < 0 || axis >= m_axisMaxCount) {
            qWarning() << QString("设置轴参数值失败：轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis);
            return ZmotionResult(false, -2, QString("轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis));
        }
        
        // 调用底层API设置轴参数值
        int result = ZAux_Direct_SetParam(m_handle, axisParamsCommandToStringMap[paramType].toStdString().c_str(), axis, paramValue);
        if (result != 0) {
            QString errorMsg = getErrorDescription(result);
            qWarning() << QString("设置轴参数值失败：轴号%1，参数类型%2，参数值%3，错误码：%4，错误信息：%5").arg(axis).arg(axisParamsCommandToStringMap[paramType]).arg(paramValue).arg(result).arg(errorMsg);
            return ZmotionResult(false, result, errorMsg);
        }
        
        // 设置返回结果
        ZmotionResult zmResult;
        zmResult.success = true;
        
        // qDebug() << QString("设置轴参数值成功：轴号%1，参数类型%2，参数值%3").arg(axis).arg(axisParamsCommandToStringMap[paramType]).arg(paramValue);
        return zmResult;
    } catch (const std::exception& e) {
        qCritical() << QString("设置轴参数值时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -3, QString("发生异常：%1").arg(e.what()));
    } catch (...) {
        qCritical() << "设置轴参数值时发生未知异常";
        return ZmotionResult(false, -4, "发生未知异常");
    }       
}

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
ZmotionResult ZmotionAPI::setAxisFsLimit(const QList<int>& axes, const QList<float>& fsLimitValues)
{
    try {
        // 检查连接状态
        if (!m_initialized || m_handle == NULL) {
            qWarning() << "设置轴正软限位失败：控制器未连接";
            return ZmotionResult(false, -1, "控制器未连接");
        }
        
        // 检查轴号列表是否为空
        if (axes.isEmpty()) {
            qWarning() << "设置轴正软限位失败：轴号列表为空";
            return ZmotionResult(false, -2, "轴号列表为空");
        }
        
        // 检查轴号列表和限位值列表长度是否匹配
        if (axes.size() != fsLimitValues.size()) {
            qWarning() << QString("设置轴正软限位失败：轴号列表长度(%1)与限位值列表长度(%2)不匹配").arg(axes.size()).arg(fsLimitValues.size());
            return ZmotionResult(false, -3, QString("轴号列表长度(%1)与限位值列表长度(%2)不匹配").arg(axes.size()).arg(fsLimitValues.size()));
        }
        
        // 验证所有轴号的有效性
        for (int axis : axes) {
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴正软限位失败：轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis);
                return ZmotionResult(false, -4, QString("轴号无效，轴号范围应为0-%1，当前值：%2").arg(m_axisMaxCount - 1).arg(axis));
            }
        }
        
        // 遍历所有轴，设置正软限位
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes[i];
            float fsLimitValue = fsLimitValues[i];
            
            // 调用ZAux_Direct_SetFsLimit API设置正软限位
            int ret = ZAux_Direct_SetFsLimit(m_handle, axis, fsLimitValue);
            if (ret != 0) {
                QString errorDesc = getErrorDescription(ret);
                qWarning() << QString("设置轴%1正软限位失败，值：%2，错误码：%3，错误描述：%4").arg(axis).arg(fsLimitValue).arg(ret).arg(errorDesc);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = ret;
                    firstErrorMessage = errorDesc;
                }
                continue; // 跳过当前轴，继续处理下一个轴
            }
            
            // 成功完成设置正软限位
            successAxes << QString::number(axis);
            ZINFO << QString("成功设置轴%1的正软限位值为%2 units").arg(axis).arg(fsLimitValue);
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置正软限位
            ZINFO << QString("成功设置轴%1的正软限位").arg(successAxes.join(","));
            return ZmotionResult(true, 0, QString("成功设置轴%1的正软限位").arg(successAxes.join(",")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴正软限位设置失败。成功轴：%1，失败轴：%2").arg(successAxes.join(",")).arg(failedAxes.join(","));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴正软限位设置失败。成功轴：%1，失败轴：%2，第一个错误：%3").
                               arg(successAxes.join(",")).arg(failedAxes.join(",")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴正软限位时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴正软限位时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/**
 * @brief ZmotionAPI::setAxisRsLimit 设置轴的负软限位
 * 
 * 该函数用于设置单个或多个轴的负向软限位值。使用ZAux_Direct_SetRsLimit API实现。
 * 轴号列表和负向限位值列表的索引必须一一对应。
 * 
 * @param axes 轴号列表（0开始）
 * @param rsLimitValues 对应的负向限位值列表，单位为units
 * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
 *         errorCode表示错误码，errorMessage表示错误描述
 */
ZmotionResult ZmotionAPI::setAxisRsLimit(const QList<int>& axes, const QList<float>& rsLimitValues)
{
    ZmotionResult result;
    
    // 初始化结果
    result.success = false;
    result.errorCode = -1;
    
    try {
        // 检查是否已连接
        if (!m_initialized || m_handle == nullptr) {
            result.errorMessage = QString("控制器未初始化或未连接");
            qCritical() << result.errorMessage;
            return result;
        }
        
        // 检查轴列表是否为空
        if (axes.isEmpty()) {
            result.errorMessage = QString("轴列表不能为空");
            qCritical() << result.errorMessage;
            return result;
        }
        
        // 检查轴号和限位值列表长度是否一致
        if (axes.size() != rsLimitValues.size()) {
            result.errorMessage = QString("轴号列表与限位值列表长度不一致");
            qCritical() << result.errorMessage;
            return result;
        }
        
        // 检查轴号是否在有效范围内
        for (int axis : axes) {
            if (axis < 0 || axis >= m_axisMaxCount) {
                result.errorMessage = QString("轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                qCritical() << result.errorMessage;
                return result;
            }
        }
        
        // 保存成功和失败的轴信息
        QList<int> successAxes;
        QList<int> failedAxes;
        QMap<int, int> errorCodes;
        
        // 遍历轴列表，设置负软限位
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            float rsLimit = rsLimitValues.at(i);
            
            // 调用ZAux_Direct_SetRsLimit设置负软限位
            int ret = ZAux_Direct_SetRsLimit(m_handle, axis, rsLimit);
            if (ret == 0) {
                successAxes.append(axis);
            } else {
                failedAxes.append(axis);
                errorCodes.insert(axis, ret);
                qWarning() << QString("轴%1设置负软限位失败，错误码：%2").arg(axis).arg(ret);
            }
        }
        
        // 设置结果
        if (failedAxes.isEmpty()) {
            result.success = true;
            result.errorCode = 0;
            result.errorMessage = QString("所有轴设置负软限位成功");
            ZINFO << result.errorMessage;
        } else {
            result.success = false;
            result.errorCode = -1;
            
            // 将QList<int>转换为字符串
            QString successAxesStr;
            for (int i = 0; i < successAxes.size(); ++i) {
                successAxesStr += QString::number(successAxes.at(i));
                if (i < successAxes.size() - 1) successAxesStr += ",";
            }
            
            QString failedAxesStr;
            for (int i = 0; i < failedAxes.size(); ++i) {
                failedAxesStr += QString::number(failedAxes.at(i));
                if (i < failedAxes.size() - 1) failedAxesStr += ",";
            }
            
            QString message = QString("部分轴设置负软限位失败。成功轴：%1，失败轴：%2。错误详情：\n").arg(
                successAxesStr, failedAxesStr);
            
            // 添加错误详情
            for (int axis : failedAxes) {
                message += QString("  轴%1：错误码%2\n").arg(axis).arg(errorCodes.value(axis));
            }
            
            result.errorMessage = message.trimmed();
            qCritical() << result.errorMessage;
        }
        
    } catch (const std::exception& e) {
        result.errorMessage = QString("异常：%1").arg(e.what());
        qCritical() << result.errorMessage;
    } catch (...) {
        result.errorMessage = QString("未知异常");
        qCritical() << result.errorMessage;
    }
    
    return result;
}

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
 */
ZmotionResult ZmotionAPI::setAxisUnits(const QList<int>& axes, const QList<double>& units)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴脉冲当量失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴脉冲当量失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和units列表长度是否匹配
        if (axes.length() != units.length()) {
            qWarning() << "设置轴脉冲当量失败：轴号列表和脉冲当量值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和脉冲当量值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double unitValue = units.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴脉冲当量失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_Direct_SetUnits API设置脉冲当量
            int result = ZAux_Direct_SetUnits(m_handle, axis, static_cast<float>(unitValue));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(unitValue);
                ZINFO << QString("成功设置轴%1的脉冲当量为%2").arg(axis).arg(unitValue);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的脉冲当量为%2失败，错误码：%3，错误描述：%4").arg(axis).arg(unitValue).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(unitValue);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的脉冲当量：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的脉冲当量：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴脉冲当量设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴脉冲当量设置失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴脉冲当量时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴脉冲当量时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::setAxisEnable(const QList<int>& axes, int enable)
{
    try {
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴使能状态失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }

        if (axes.isEmpty()) {
            qWarning() << "设置轴使能状态失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }

        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴使能状态失败：轴号%1超出有效范围").arg(axis);
                return ZmotionResult(false, -1, QString("轴号%1超出有效范围").arg(axis));
            }

            int result = ZAux_Direct_SetAxisEnable(m_handle, axis, enable);
            if (result != 0) {
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1使能状态失败，错误码：%2").arg(axis).arg(result);
                return ZmotionResult(false, result, errorDesc);
            }
            ZINFO << QString("成功设置轴%1使能状态为%2").arg(axis).arg(enable);
        }

        return ZmotionResult(true, 0, QString("成功设置所有轴的使能状态为%1").arg(enable));
    }
    catch (const std::exception& e) {
        qCritical() << QString("设置轴使能状态时发生异常：%1").arg(e.what());
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(e.what()));
    }
    catch (...) {
        qCritical() << "设置轴使能状态时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
 */
ZmotionResult ZmotionAPI::setAxisAccel(const QList<int>& axes, const QList<double>& accelerations)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴加速度失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴加速度失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和accelerations列表长度是否匹配
        if (axes.length() != accelerations.length()) {
            qWarning() << "设置轴加速度失败：轴号列表和加速度值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和加速度值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double accelValue = accelerations.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴加速度失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_Direct_SetAccel API设置加速度
            int result = ZAux_Direct_SetAccel(m_handle, axis, static_cast<float>(accelValue));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(accelValue);
                ZINFO << QString("成功设置轴%1的加速度为%2 units/s/s").arg(axis).arg(accelValue);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的加速度为%2失败，错误码：%3，错误描述：%4").arg(axis).arg(accelValue).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(accelValue);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的加速度：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的加速度：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴加速度设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴加速度设置失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴加速度时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴加速度时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
 */
ZmotionResult ZmotionAPI::setAxisDecel(const QList<int>& axes, const QList<double>& decelerations)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴减速度失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴减速度失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和decelerations列表长度是否匹配
        if (axes.length() != decelerations.length()) {
            qWarning() << "设置轴减速度失败：轴号列表和减速度值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和减速度值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double decelValue = decelerations.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴减速度失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_Direct_SetDecel API设置减速度
            int result = ZAux_Direct_SetDecel(m_handle, axis, static_cast<float>(decelValue));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(decelValue);
                ZINFO << QString("成功设置轴%1的减速度为%2 units/s/s").arg(axis).arg(decelValue);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的减速度为%2失败，错误码：%3，错误描述：%4").arg(axis).arg(decelValue).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(decelValue);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的减速度：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的减速度：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴减速度设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴减速度设置失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴减速度时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴减速度时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::setAxisSpeed(const QList<int>& axes, const QList<double>& speeds)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴速度失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴速度失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和speeds列表长度是否匹配
        if (axes.length() != speeds.length()) {
            qWarning() << "设置轴速度失败：轴号列表和速度值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和速度值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double speedValue = speeds.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴速度失败：轴号%1超出有效范围[0-%2]")
                           .arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(speedValue);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_Direct_SetSpeed API设置速度
            int result = ZAux_Direct_SetSpeed(m_handle, axis, static_cast<float>(speedValue));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(speedValue);
                ZINFO << QString("成功设置轴%1的速度为%2 units/s").arg(axis).arg(speedValue);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的速度为%2失败，错误码：%3，错误描述：%4")
                           .arg(axis).arg(speedValue).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(speedValue);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的速度：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的速度：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴速度设置失败。成功：%1，失败：%2")
                      .arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴速度设置失败。成功：%1，失败：%2，第一个错误：%3")
                               .arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴速度时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴速度时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
    ZmotionResult ZmotionAPI::setAxisFastDecel(const QList<int>& axes, const QList<double>& fastDecelerations)
    {
        try {
            // 检查设备连接状态
            if (!isConnected()) {
                qCritical() << "设置轴快速减速度失败：设备未连接或未初始化";
                return ZmotionResult(false, -1, "设备未连接或未初始化");
            }

            // 参数验证
            if (axes.isEmpty()) {
                qCritical() << "设置轴快速减速度失败：轴号列表为空";
                return ZmotionResult(false, -2, "轴号列表为空");
            }

            if (axes.size() != fastDecelerations.size()) {
                qCritical() << "设置轴快速减速度失败：轴号列表与快速减速度列表长度不匹配";
                return ZmotionResult(false, -3, "轴号列表与快速减速度列表长度不匹配");
            }

            // 验证轴号范围
            for (int i = 0; i < axes.size(); ++i) {
                if (axes[i] < 0 || axes[i] >= m_axisMaxCount) {
                    qCritical() << QString("设置轴快速减速度失败：轴号 %1 超出有效范围 [0, %2]")
                                 .arg(axes[i]).arg(m_axisMaxCount - 1);
                    return ZmotionResult(false, -4, QString("轴号 %1 超出有效范围 [0, %2]")
                                        .arg(axes[i]).arg(m_axisMaxCount - 1));
                }
            }

            QList<int> failedAxes;
            QStringList errorMessages;
            int lastErrorCode = 0;

            // 遍历所有轴，设置快速减速度
            for (int i = 0; i < axes.size(); ++i) {
                int axis = axes[i];
                double fastDecel = fastDecelerations[i];
                
                // 调用ZAux_Direct_SetFastDec API设置快速减速度
                int result = ZAux_Direct_SetFastDec(m_handle, axis, static_cast<float>(fastDecel));
                
                if (result != 0) {
                    failedAxes.append(axis);
                    lastErrorCode = result;
                    QString errorMsg = getErrorDescription(result);
                    errorMessages.append(QString("轴 %1 设置快速减速度失败: %2 (错误码: %3)")
                                        .arg(axis).arg(errorMsg).arg(result));
                    qWarning() << errorMessages.last();
                } else {
                    ZINFO << QString("轴 %1 快速减速度设置成功: %2 units/s/s")
                             .arg(axis).arg(fastDecel);
                }
            }

            // 汇总结果
            if (failedAxes.isEmpty()) {
                // 所有轴设置成功
                QString successMsg = "所有轴快速减速度设置成功";
                if (axes.size() == 1) {
                    successMsg = QString("轴 %1 快速减速度设置成功: %2 units/s/s")
                                .arg(axes.first()).arg(fastDecelerations.first());
                }
                ZINFO << successMsg;
                return ZmotionResult(true, 0, successMsg);
            } else {
                // 部分或全部轴设置失败
                QStringList failedAxisStrs;
                for (int axis : failedAxes) {
                    failedAxisStrs.append(QString::number(axis));
                }
                QString failedMsg = QString("部分轴快速减速度设置失败: %1")
                                   .arg(failedAxisStrs.join(", "));
                qCritical() << failedMsg;
                return ZmotionResult(false, lastErrorCode, failedMsg);
            }
        } catch (const std::exception& e) {
            // 捕获标准异常
            QString errorMsg = QString("设置轴快速减速度时发生异常: %1")
                              .arg(QString::fromUtf8(e.what()));
            qCritical() << errorMsg;
            return ZmotionResult(false, -100, errorMsg);
        } catch (...) {
            // 捕获未知异常
            QString errorMsg = "设置轴快速减速度时发生未知异常";
            qCritical() << errorMsg;
            return ZmotionResult(false, -101, errorMsg);
        }
    }

/**
 * @brief ZmotionAPI::setAxisLspeed 设置轴的起始速度
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
 */
ZmotionResult ZmotionAPI::setAxisLspeed(const QList<int>& axes, const QList<double>& speeds)
{
    ZmotionResult result;
    
    // 检查连接状态
    if (!m_initialized || m_handle == nullptr) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 检查参数有效性
    if (axes.isEmpty()) {
        result.success = false;
        result.errorCode = -2;
        result.errorMessage = "轴号列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }
    
    if (axes.size() != speeds.size()) {
        result.success = false;
        result.errorCode = -3;
        result.errorMessage = QString("轴号列表和起始速度值列表长度不匹配: 轴号列表长度=%1, 速度值列表长度=%2")
                             .arg(axes.size()).arg(speeds.size());
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 验证轴号范围
    for (int axis : axes) {
        if (axis < 0 || axis > m_axisMaxCount - 1) {
            result.success = false;
            result.errorCode = -4;
            result.errorMessage = QString("轴号无效: %1，有效范围为0-%2")
                                 .arg(axis).arg(m_axisMaxCount - 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }
    
    // 遍历每个轴设置起始速度
    bool allSuccess = true;
    QList<int> failedAxes;
    int lastErrorCode = 0;
    
    try {
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double speed = speeds.at(i);
            
            // 调用ZAux_Direct_SetLspeed API设置起始速度
            int ret = ZAux_Direct_SetLspeed(m_handle, axis, static_cast<float>(speed));
            
            if (ret != 0) {
                allSuccess = false;
                failedAxes.append(axis);
                lastErrorCode = ret;
                qWarning() << QString("设置轴%1的起始速度失败，错误码: %2")
                           .arg(axis).arg(ret);
            } else {
                ZINFO << QString("成功设置轴%1的起始速度为: %2 units/s")
                        .arg(axis).arg(speed);
            }
        }
    } catch (const std::exception& e) {
        // 捕获异常
        result.success = false;
        result.errorCode = -5;
        result.errorMessage = QString("设置起始速度时发生异常: %1")
                             .arg(e.what());
        qCritical() << result.errorMessage;
        return result;
    } catch (...) {
        // 捕获未知异常
        result.success = false;
        result.errorCode = -6;
        result.errorMessage = "设置起始速度时发生未知异常";
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 设置结果
    result.success = allSuccess;
    
    if (allSuccess) {
        result.errorCode = 0;
        result.errorMessage = "所有轴的起始速度设置成功";
    } else {
        result.errorCode = lastErrorCode;
        // 构建失败轴的错误消息
        QStringList axisStrings;
        for (int axis : failedAxes) {
            axisStrings.append(QString::number(axis));
        }
        result.errorMessage = QString("以下轴的起始速度设置失败: %1，错误码: %2")
                             .arg(axisStrings.join(", ")).arg(lastErrorCode);
    }
    
    return result;
}

/**
 * @brief ZmotionAPI::setAxisSramp 设置轴的S曲线时间
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
 */
ZmotionResult ZmotionAPI::setAxisSramp(const QList<int>& axes, const QList<double>& srampValues)
{
    ZmotionResult result;
    
    // 检查连接状态
    if (!m_initialized || m_handle == nullptr) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 检查参数有效性
    if (axes.isEmpty()) {
        result.success = false;
        result.errorCode = -2;
        result.errorMessage = "轴号列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }
    
    if (axes.size() != srampValues.size()) {
        result.success = false;
        result.errorCode = -3;
        result.errorMessage = QString("轴号列表和S曲线时间值列表长度不匹配: 轴号列表长度=%1, S曲线时间值列表长度=%2")
                             .arg(axes.size()).arg(srampValues.size());
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 验证轴号范围
    for (int axis : axes) {
        if (axis < 0 || axis > m_axisMaxCount - 1) {
            result.success = false;
            result.errorCode = -4;
            result.errorMessage = QString("轴号无效: %1，有效范围为0-%2")
                                 .arg(axis).arg(m_axisMaxCount - 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }
    
    // 遍历每个轴设置S曲线时间
    bool allSuccess = true;
    QList<int> failedAxes;
    int lastErrorCode = 0;
    
    try {
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double srampValue = srampValues.at(i);
            
            // 调用ZAux_Direct_SetSramp API设置S曲线时间
            int ret = ZAux_Direct_SetSramp(m_handle, axis, static_cast<float>(srampValue));
            
            if (ret != 0) {
                allSuccess = false;
                failedAxes.append(axis);
                lastErrorCode = ret;
                qWarning() << QString("设置轴%1的S曲线时间失败，错误码: %2")
                           .arg(axis).arg(ret);
            } else {
                if (srampValue == 0) {
                    ZINFO << QString("成功设置轴%1为梯形加减速")
                            .arg(axis);
                } else {
                    ZINFO << QString("成功设置轴%1的S曲线时间为: %2 ms")
                            .arg(axis).arg(srampValue);
                }
            }
        }
    } catch (const std::exception& e) {
        // 捕获异常
        result.success = false;
        result.errorCode = -5;
        result.errorMessage = QString("设置S曲线时间时发生异常: %1")
                             .arg(e.what());
        qCritical() << result.errorMessage;
        return result;
    } catch (...) {
        // 捕获未知异常
        result.success = false;
        result.errorCode = -6;
        result.errorMessage = "设置S曲线时间时发生未知异常";
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 设置结果
    result.success = allSuccess;
    
    if (allSuccess) {
        result.errorCode = 0;
        result.errorMessage = "所有轴的S曲线时间设置成功";
    } else {
        result.errorCode = lastErrorCode;
        // 构建失败轴的错误消息
        QStringList axisStrings;
        for (int axis : failedAxes) {
            axisStrings.append(QString::number(axis));
        }
        result.errorMessage = QString("以下轴的S曲线时间设置失败: %1，错误码: %2")
                             .arg(axisStrings.join(", ")).arg(lastErrorCode);
    }
    
    return result;
}

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
*/
ZmotionResult ZmotionAPI::setAxisSingleMove(const QList<int>& axes, const QList<double>& distances)
{
    ZmotionResult result;
    
    try {
        // 检查连接状态
        if (!isConnected()) {
            result.success = false;
            result.errorCode = -1;
            result.errorMessage = "控制器未连接或未初始化";
            qWarning() << result.errorMessage;
            return result;
        }

        // 参数验证
        if (axes.isEmpty() || distances.isEmpty()) {
            result.success = false;
            result.errorCode = -2;
            result.errorMessage = "轴号列表或距离值列表不能为空";
            qWarning() << result.errorMessage;
            return result;
        }
        
        if (axes.size() != distances.size()) {
            result.success = false;
            result.errorCode = -3;
            result.errorMessage = "轴号列表与距离值列表长度不匹配";
            qWarning() << result.errorMessage;
            return result;
        }
        
        // 存储每个轴的执行结果
        QStringList errorMessages;
        bool allSuccess = true;
        int lastErrorCode = 0;
        
        // 遍历所有轴，执行相对运动
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double distance = distances.at(i);
            
            // 验证轴号范围
            if (axis < 0 || axis >= 256) {  // 假设最大轴号为255
                result.success = false;
                result.errorCode = -4;
                result.errorMessage = QString("轴号 %1 超出有效范围 [0-255]").arg(axis);
                qWarning() << result.errorMessage;
                return result;
            }
            
            // 执行相对运动指令
            int ret = ZAux_Direct_Single_Move(m_handle, axis, static_cast<float>(distance));
            if (ret != 0) {
                allSuccess = false;
                lastErrorCode = ret;
                errorMessages.append(QString("轴%1 相对运动失败，错误码: %2").arg(axis).arg(ret));
                qWarning() << QString("轴%1 相对运动失败，错误码: %2").arg(axis).arg(ret);
            } else {
                ZINFO << QString("轴%1 相对运动指令发送成功，距离值: %2").arg(axis).arg(distance);
            }
        }
        
        // 设置最终结果
        if (allSuccess) {
            result.success = true;
            result.errorCode = 0;
            result.errorMessage = "所有轴相对运动指令发送成功";
            ZINFO << result.errorMessage;
        } else {
            result.success = false;
            result.errorCode = lastErrorCode;
            result.errorMessage = errorMessages.join(", ");
        }
    } catch (const std::exception& e) {
        // 捕获C++异常
        result.success = false;
        result.errorCode = -999;
        result.errorMessage = QString("执行异常: %1").arg(QString::fromUtf8(e.what()));
        qCritical() << result.errorMessage;
    } catch (...) {
        // 捕获未知异常
        result.success = false;
        result.errorCode = -1000;
        result.errorMessage = "执行时发生未知异常";
        qCritical() << result.errorMessage;
    }
    
    return result;
}

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
 */
ZmotionResult ZmotionAPI::setAxisSingleMoveAbs(const QList<int>& axes, const QList<double>& positions)
{
    ZmotionResult result;
    
    try {
        // 检查连接状态
        if (!isConnected()) {
            result.success = false;
            result.errorCode = -1;
            result.errorMessage = "控制器未连接或未初始化";
            qWarning() << result.errorMessage;
            return result;
        }

        // 参数验证
        if (axes.isEmpty() || positions.isEmpty()) {
            result.success = false;
            result.errorCode = -2;
            result.errorMessage = "轴号列表或位置值列表不能为空";
            qWarning() << result.errorMessage;
            return result;
        }
        
        if (axes.size() != positions.size()) {
            result.success = false;
            result.errorCode = -3;
            result.errorMessage = "轴号列表与位置值列表长度不匹配";
            qWarning() << result.errorMessage;
            return result;
        }
        
        // 存储每个轴的执行结果
        QStringList errorMessages;
        bool allSuccess = true;
        int lastErrorCode = 0;
        
        // 遍历所有轴，执行绝对运动
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double position = positions.at(i);
            
            // 验证轴号范围
            if (axis < 0 || axis >= 256) {  // 假设最大轴号为255
                result.success = false;
                result.errorCode = -4;
                result.errorMessage = QString("轴号 %1 超出有效范围 [0-255]").arg(axis);
                qWarning() << result.errorMessage;
                return result;
            }
            
            // 执行绝对运动指令
            int ret = ZAux_Direct_Single_MoveAbs(m_handle, axis, static_cast<float>(position));
            if (ret != 0) {
                allSuccess = false;
                lastErrorCode = ret;
                errorMessages.append(QString("轴%1 绝对运动失败，错误码: %2").arg(axis).arg(ret));
                qWarning() << QString("轴%1 绝对运动失败，错误码: %2").arg(axis).arg(ret);
            } else {
                ZINFO << QString("轴%1 绝对运动指令发送成功，位置值: %2").arg(axis).arg(position);
            }
        }
        
        // 设置最终结果
        if (allSuccess) {
            result.success = true;
            result.errorCode = 0;
            result.errorMessage = "所有轴绝对运动指令发送成功";
            ZINFO << result.errorMessage;
        } else {
            result.success = false;
            result.errorCode = lastErrorCode;
            result.errorMessage = errorMessages.join(", ");
        }
    } catch (const std::exception& e) {
        // 捕获C++异常
        result.success = false;
        result.errorCode = -999;
        result.errorMessage = QString("执行异常: %1").arg(QString::fromUtf8(e.what()));
        qCritical() << result.errorMessage;
    } catch (...) {
        // 捕获未知异常
        result.success = false;
        result.errorCode = -1000;
        result.errorMessage = "执行时发生未知异常";
        qCritical() << result.errorMessage;
    }
    
    return result;
}

/**
 * @brief 单轴或多轴持续运动控制
 * @param axes 轴号列表，支持单轴或多轴控制
 * @param directions 方向列表，-1表示负向，1表示正向，与axes列表索引一一对应
 * @return ZmotionResult 操作结果，包含错误码和错误信息
 */
ZmotionResult ZmotionAPI::setAxisSingleVmove(const QList<int>& axes, const QList<int>& directions)
{
    ZmotionResult result;
    
    // 检查连接状态
    if (!m_initialized || m_handle == nullptr) {
        result.errorCode = -1;
        result.errorMessage = QStringLiteral("控制器未连接");
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 检查参数有效性
    if (axes.isEmpty()) {
        result.errorCode = -2;
        result.errorMessage = QStringLiteral("轴号列表不能为空");
        qWarning() << result.errorMessage;
        return result;
    }
    
    if (axes.size() != directions.size()) {
        result.errorCode = -3;
        result.errorMessage = QStringLiteral("轴号列表与方向列表长度不匹配");
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 验证所有方向参数是否有效
    for (int dir : directions) {
        if (dir != -1 && dir != 1) {
            result.errorCode = -4;
            result.errorMessage = QStringLiteral("方向参数无效，必须为-1（负向）或1（正向）");
             qWarning() << result.errorMessage;
            return result;
        }
    }
    
    // 验证轴号范围（假设控制器支持0-7轴，根据实际情况调整）
    for (int axis : axes) {
        if (axis < 0 || axis > m_axisMaxCount - 1) {
            result.errorCode = -5;
            result.errorMessage = QStringLiteral("轴号超出有效范围[0-%1]").arg(m_axisMaxCount - 1);
             qWarning() << result.errorMessage << "，轴号:" << axis;
            return result;
        }
    }
    
    // 执行持续运动命令
    int lastErrorCode = 0;
    QStringList errorMessages;
    
    try {
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            int dir = directions.at(i);
            
            int ret = ZAux_Direct_Single_Vmove(m_handle, axis, dir);
            if (ret != 0) {
                lastErrorCode = ret;
                errorMessages.append(QStringLiteral("轴%1持续运动失败，错误码:%2").arg(axis).arg(ret));
                qWarning() << QStringLiteral("轴%1以方向%2进行持续运动失败，错误码:%3").arg(axis).arg(dir).arg(ret);
            } else {
                ZINFO << QStringLiteral("轴%1以方向%2开始持续运动").arg(axis).arg(dir);
            }
        }
    } catch (const std::exception& e) {
        lastErrorCode = -6;
        result.errorCode = lastErrorCode;
        result.errorMessage = QStringLiteral("执行过程中发生异常:%1").arg(e.what());
        qCritical() << result.errorMessage;
        return result;
    } catch (...) {
        lastErrorCode = -7;
        result.errorCode = lastErrorCode;
        result.errorMessage = QStringLiteral("执行过程中发生未知异常");
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 设置结果
    if (errorMessages.isEmpty()) {
        result.errorCode = 0;
        result.errorMessage = QStringLiteral("持续运动命令执行成功");
    } else {
        result.errorCode = lastErrorCode;
        result.errorMessage = errorMessages.join("；");
    }
    
    return result;
}

/**
 * @brief 设置轴的映射原点，核心指令ZAux_Direct_SetDatumIn
 * @param axes 轴号列表（0开始），支持单个或多个轴号
 * @param originsInValue IO口编号，映射原点值列表，与axes列表索引一一对应，-1表示取消映射原点
 * @return ZmotionResult 包含执行结果的结构体，success表示是否成功，
 *         errorCode表示错误码，errorMessage表示错误描述
 * @note 映射原点是轴的参考位置，用于回零运动和绝对运动。
 *       设置映射原点后，轴将从该位置开始运动。
 *       -1 表示取消映射原点，轴将从当前位置开始运动。
 * @note 请确保axes和originsInValue列表的长度相同，每个轴对应一个映射原点值
 */
ZmotionResult ZmotionAPI::setAxisDatumOrigin(const QList<int>& axes, const QList<int>& originsInValue)
{
    ZmotionResult result;
    try{
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴爬行速度失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴爬行速度失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和originsInValue列表长度是否匹配
        if (axes.length() != originsInValue.length()) {
            qWarning() << "设置轴映射原点失败：轴号列表和映射原点值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和映射原点值列表长度不匹配");
        }

        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;

        // 遍历轴号列表，设置每个轴的映射原点
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            int origin = originsInValue.at(i);
            
            int ret = ZAux_Direct_SetDatumIn(m_handle, axis, origin);
            if (ret != 0) {
                failedAxes.append(QString::number(axis));
                if (firstErrorCode == 0) {
                    firstErrorCode = ret;
                    firstErrorMessage = QStringLiteral("轴%1设置映射原点失败，错误码:%2").arg(axis).arg(ret);
                }
            } else {
                successAxes.append(QString::number(axis));
            }
        }
        //返回结果
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            QString successMsg = QStringLiteral("成功设置所有轴的映射原点：%1").arg(successAxes.join(", "));
            ZINFO << successMsg;
            return ZmotionResult(true, 0, successMsg);
        } else {
            // 部分轴设置失败
            QString warningMsg = QStringLiteral("部分轴映射原点设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            QString errorMsg = QStringLiteral("部分轴映射原点设置失败。成功：%1，失败：%2，第一个错误：%3").arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage);
            qWarning() << warningMsg;
            return ZmotionResult(false, firstErrorCode, errorMsg);
        }

    }catch(const std::exception& e){
        result.errorCode = -1;
        result.errorMessage = QStringLiteral("设置轴映射原点失败:%1").arg(e.what());
        qCritical() << result.errorMessage;
        return result;
    }catch(...){
        result.errorCode = -2;
        result.errorMessage = QStringLiteral("设置轴映射原点失败：未知异常");
        qCritical() << result.errorMessage;
        return result;
    }
    
}



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
 */
ZmotionResult ZmotionAPI::setAxisCreepSpeed(const QList<int>& axes, const QList<double>& speeds)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴爬行速度失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴爬行速度失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和speeds列表长度是否匹配
        if (axes.length() != speeds.length()) {
            qWarning() << "设置轴爬行速度失败：轴号列表和爬行速度值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和爬行速度值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double creepSpeed = speeds.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴爬行速度失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_Direct_SetCreep API设置爬行速度
            int result = ZAux_Direct_SetCreep(m_handle, axis, static_cast<float>(creepSpeed));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(creepSpeed);
                ZINFO << QString("成功设置轴%1的爬行速度为%2").arg(axis).arg(creepSpeed);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的爬行速度为%2失败，错误码：%3，错误描述：%4").arg(axis).arg(creepSpeed).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(creepSpeed);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的爬行速度：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的爬行速度：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴爬行速度设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴爬行速度设置失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴爬行速度时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴爬行速度时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::setAxisDatumOffset(const QList<int>& axes, const QList<double>& offsets)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴回零偏移距离失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "设置轴回零偏移距离失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和offsets列表长度是否匹配
        if (axes.length() != offsets.length()) {
            qWarning() << "设置轴回零偏移距离失败：轴号列表和回零偏移距离值列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和回零偏移距离值列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行设置
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            double offset = offsets.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("设置轴回零偏移距离失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_BusCmd_SetDatumOffpos API设置回零偏移距离
            int result = ZAux_BusCmd_SetDatumOffpos(m_handle, axis, static_cast<float>(offset));
            
            if (result == 0) {
                // 设置成功
                successAxes << QString("轴%1(值:%2)").arg(axis).arg(offset);
                ZINFO << QString("成功设置轴%1的回零偏移距离为%2").arg(axis).arg(offset);
            } else {
                // 设置失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("设置轴%1的回零偏移距离为%2失败，错误码：%3，错误描述：%4").arg(axis).arg(offset).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(值:%2)").arg(axis).arg(offset);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功设置
            ZINFO << QString("成功设置所有轴的回零偏移距离：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功设置所有轴的回零偏移距离：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴设置失败
            qWarning() << QString("部分轴回零偏移距离设置失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴回零偏移距离设置失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴回零偏移距离时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴回零偏移距离时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/**
 * @brief setAxisSingleDatum 设置轴单轴回零
 * 
 * @param axisIndex 轴号
 * @param homeMode 回零模式
 * @return ZmotionResult 操作结果
 * @param HighSpeed 回零高速：回零时的高速运动速度，单位为脉冲当量单位/s
 * @param LowSpeed 回零低速：回零时的低速运动速度，单位为脉冲当量单位/s
 * @param Datumoffset 回零偏移距离：回零完成后对轴的位置进行偏移调整的距离，单位为脉冲当量单位
 */
ZmotionResult ZmotionAPI::setAxisSingleDatum(int axisIndex, int homeMode, float HighSpeed, float LowSpeed, float Datumoffset)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "设置轴单轴回零失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查轴号有效性
        if (axisIndex < 0 || axisIndex >= m_axisMaxCount) {
            qWarning() << QString("设置轴单轴回零失败：轴号%1超出有效范围[0-%2]").arg(axisIndex).arg(m_axisMaxCount - 1);
            return ZmotionResult(false, -1, QString("轴号%1超出有效范围").arg(axisIndex));
        }

        //设置控制器回零等待时间
        // ZAux_Direct_SetHomeWait(m_handle,axisIndex,5000);
        
        // ZAux_Direct_Single_Datum API设置单轴回零
        int result = ZAux_Direct_UserDatum(m_handle, axisIndex, homeMode, HighSpeed, LowSpeed, Datumoffset);
        
        if (result == 0) {
            // 设置成功
            ZINFO << QString("成功设置轴%1的单轴回零模式为%2").arg(axisIndex).arg(homeMode);
            return ZmotionResult(true, 0, QString("成功设置轴%1的单轴回零模式为%2").arg(axisIndex).arg(homeMode));
        } else {
            // 设置失败
            QString errorDesc = getErrorDescription(result);
            qWarning() << QString("设置轴%1的单轴回零模式为%2失败，错误码：%3，错误描述：%4").arg(axisIndex).arg(homeMode).arg(result).arg(errorDesc);
            return ZmotionResult(false, result, QString("设置轴%1的单轴回零模式为%2失败，错误码：%3，错误描述：%4").arg(axisIndex).arg(homeMode).arg(result).arg(errorDesc));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("设置轴单轴回零时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "设置轴单轴回零时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }   
}

/**
 * @brief getAxisSingleHomeStatus 获取轴单轴回零状态
 * 
 * @param axisIndex 轴号
 * @return ZmotionResult 操作结果
 */
ZmotionResult ZmotionAPI::getAxisSingleHomeStatus(int axisIndex)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "获取轴单轴回零状态失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查轴号有效性
        if (axisIndex < 0 || axisIndex >= m_axisMaxCount) {
            qWarning() << QString("获取轴单轴回零状态失败：轴号%1超出有效范围[0-%2]").arg(axisIndex).arg(m_axisMaxCount - 1);
            return ZmotionResult(false, -1, QString("轴号%1超出有效范围").arg(axisIndex));
        }
        
        // ZAux_Direct_GetHomeStatus API获取单轴回零状态
        uint32 homeStatus = 0;
        int result = ZAux_Direct_GetHomeStatus(m_handle, axisIndex, &homeStatus);
        
        if (result == 0) {
            // 获取成功
            //ZINFO << QString("成功获取轴%1的单轴回零状态为%2").arg(axisIndex).arg(homeStatus);
            return ZmotionResult(true, 0, QString("成功获取轴%1的单轴回零状态为%2").arg(axisIndex).arg(homeStatus), QStringList(), QList<int>(), 0, 0.0, homeStatus);
        } else {
            // 获取失败
            QString errorDesc = getErrorDescription(result);
            qWarning() << QString("获取轴%1的单轴回零状态失败，错误码：%2，错误描述：%3").arg(axisIndex).arg(result).arg(errorDesc);
            return ZmotionResult(false, result, QString("获取轴%1的单轴回零状态失败，错误码：%2，错误描述：%3").arg(axisIndex).arg(result).arg(errorDesc));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("获取轴单轴回零状态时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "获取轴单轴回零状态时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::startAxisBusDatum(const QList<int>& axes, const QList<int>& homeModes)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "启动总线驱动器回零失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "启动总线驱动器回零失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 检查axes和homeModes列表长度是否匹配
        if (axes.length() != homeModes.length()) {
            qWarning() << "启动总线驱动器回零失败：轴号列表和回零模式列表长度不匹配";
            return ZmotionResult(false, -1, "轴号列表和回零模式列表长度不匹配");
        }
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        
        // 遍历每个轴进行回零操作
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            int homeMode = homeModes.at(i);
            
            // 验证轴号有效性
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("启动总线驱动器回零失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_BusCmd_Datum API启动总线驱动器回零
            int result = ZAux_BusCmd_Datum(m_handle, static_cast<uint32_t>(axis), static_cast<uint32_t>(homeMode));
            
            if (result == 0) {
                // 回零启动成功
                successAxes << QString("轴%1(回零模式:%2)").arg(axis).arg(homeMode);
                ZINFO << QString("成功启动轴%1的总线驱动器回零，回零模式：%2").arg(axis).arg(homeMode);
            } else {
                // 回零启动失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("启动轴%1的总线驱动器回零失败，回零模式：%2，错误码：%3，错误描述：%4").arg(axis).arg(homeMode).arg(result).arg(errorDesc);
                failedAxes << QString("轴%1(回零模式:%2)").arg(axis).arg(homeMode);
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功启动回零
            ZINFO << QString("成功启动所有轴的总线驱动器回零：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功启动所有轴的总线驱动器回零：%1").arg(successAxes.join(", ")));
        } else {
            // 部分轴启动回零失败
            qWarning() << QString("部分轴总线驱动器回零启动失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴总线驱动器回零启动失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage));
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("启动总线驱动器回零时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "启动总线驱动器回零时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

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
ZmotionResult ZmotionAPI::getAxisBusHomeStatus(const QList<int>& axes)
{
    try {
        // 检查设备连接状态
        if (m_handle == NULL || !m_initialized) {
            qCritical() << "获取总线驱动器回零状态失败：设备未连接或未初始化";
            return ZmotionResult(false, -1, "设备未连接或未初始化");
        }
        
        // 检查参数有效性
        if (axes.isEmpty()) {
            qWarning() << "获取总线驱动器回零状态失败：轴号列表不能为空";
            return ZmotionResult(false, -1, "轴号列表不能为空");
        }
        
        // 初始化homeStatusList，大小与axes列表相同
        QList<int> homeStatusList;
        
        // 用于存储成功和失败的轴信息
        QStringList successAxes;
        QStringList failedAxes;
        int firstErrorCode = 0;
        QString firstErrorMessage;
        bool hasSuccess = false;
        
        // 遍历每个轴获取回零状态
        for (int i = 0; i < axes.size(); ++i) {
            int axis = axes.at(i);
            
            // 验证轴号有效性，确保不超过最大轴数
            if (axis < 0 || axis >= m_axisMaxCount) {
                qWarning() << QString("获取总线驱动器回零状态失败：轴号%1超出有效范围[0-%2]").arg(axis).arg(m_axisMaxCount - 1);
                failedAxes << QString::number(axis);
                homeStatusList.append(-1); // 添加无效值表示错误
                if (firstErrorCode == 0) {
                    firstErrorCode = -1;
                    firstErrorMessage = QString("轴号%1超出有效范围").arg(axis);
                }
                continue;
            }
            
            // 调用ZAux_BusCmd_GetHomeStatus API获取回零状态
            uint32_t homeState = 0;
            int result = ZAux_BusCmd_GetHomeStatus(m_handle, static_cast<uint32_t>(axis), &homeState);
            
            if (result == 0) {
                // 获取状态成功
                homeStatusList.append(static_cast<int>(homeState));
                successAxes << QString("轴%1(状态:%2)").arg(axis).arg(homeState);
                ZINFO << QString("成功获取轴%1的总线驱动器回零状态：%2").arg(axis).arg(homeState);
                hasSuccess = true;
            } else {
                // 获取状态失败
                QString errorDesc = getErrorDescription(result);
                qWarning() << QString("获取轴%1的总线驱动器回零状态失败，错误码：%2，错误描述：%3").arg(axis).arg(result).arg(errorDesc);
                failedAxes << QString::number(axis);
                homeStatusList.append(-1); // 添加无效值表示错误
                if (firstErrorCode == 0) {
                    firstErrorCode = result;
                    firstErrorMessage = errorDesc;
                }
            }
        }
        
        // 根据执行结果返回相应的状态
        if (failedAxes.isEmpty()) {
            // 所有轴都成功获取状态
            ZINFO << QString("成功获取所有轴的总线驱动器回零状态：%1").arg(successAxes.join(", "));
            return ZmotionResult(true, 0, QString("成功获取所有轴的总线驱动器回零状态：%1").arg(successAxes.join(", ")), QStringList(), homeStatusList);
        } else if (hasSuccess) {
            // 部分轴获取状态成功
            qWarning() << QString("部分轴总线驱动器回零状态获取失败。成功：%1，失败：%2").arg(successAxes.join(", ")).arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("部分轴总线驱动器回零状态获取失败。成功：%1，失败：%2，第一个错误：%3").
                               arg(successAxes.join(", ")).arg(failedAxes.join(", ")).arg(firstErrorMessage),
                               QStringList(), homeStatusList);
        } else {
            // 所有轴都获取状态失败
            qCritical() << QString("所有轴总线驱动器回零状态获取失败：%1").arg(failedAxes.join(", "));
            return ZmotionResult(false, firstErrorCode, 
                               QString("所有轴总线驱动器回零状态获取失败：%1，第一个错误：%2").
                               arg(failedAxes.join(", ")).arg(firstErrorMessage),
                               QStringList(), homeStatusList);
        }
    }
    catch (const std::exception &e) {
        qCritical() << QString("获取总线驱动器回零状态时发生异常：%1").arg(QString::fromUtf8(e.what()));
        return ZmotionResult(false, -1, QString("发生异常：%1").arg(QString::fromUtf8(e.what())));
    }
    catch (...) {
        qCritical() << "获取总线驱动器回零状态时发生未知异常";
        return ZmotionResult(false, -1, "发生未知异常");
    }
}

/**
* @brief directSetMerge 设置单轴的连续插补开关
* @param axis 轴号
* @param enable true-打开连续插补，false-关闭连续插补
* @return ZmotionResult 包含执行状态、错误码和错误信息
* @note 连续插补功能允许多段插补之间不减速，提高运动效率
*       但在某些情况下即使设置为ON也可能仍然减速，请参考API手册
*/
ZmotionResult ZmotionAPI::directSetMerge(int axis, bool enable)
{
    ZmotionResult result;
    result.success = false;
    result.errorCode = 0;
    result.errorMessage = "";

    // 检查是否已经初始化
    if (!m_initialized) {
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查是否已经连接
    if (m_handle == nullptr) {
        result.errorCode = -2;
        result.errorMessage = "控制器未连接";
        qCritical() << result.errorMessage;
        return result;
    }

    // 参数范围检查
    if (axis < 0 || axis >= m_axisMaxCount) {
        result.errorCode = -3;
        result.errorMessage = QString("轴号 %1 超出有效范围 [0-%2]")
                                .arg(axis).arg(m_axisMaxCount - 1);
        qWarning() << result.errorMessage;
        return result;
    }

    // 将bool转换为int值（1-打开，0-关闭）
    int mergeValue = enable ? 1 : 0;
    ZINFO << QString("ZmotionAPI::directSetMerge: 设置轴%1的连续插补开关为%2")
               .arg(axis).arg(mergeValue ? "ON" : "OFF");

    // 调用ZAux_Direct_SetMerge API
    int32_t ret = ZAux_Direct_SetMerge(m_handle, axis, mergeValue);
    
    // 检查返回结果
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置连续插补开关失败，错误码: %1")
                                .arg(ret);
        qWarning() << result.errorMessage;
        return result;
    }
    
    // 设置成功
    result.success = true;
    result.errorCode = 0;
    return result;
}

/**
 * @brief ZmotionAPI::directSetCornerMode 设置拐角模式
 * @param axis 轴号（主轴）
 * @param mode 模式设置值，不同位代表不同功能（可组合使用）
 * @return ZmotionResult 包含执行状态、错误码和错误信息
 */
ZmotionResult ZmotionAPI::directSetCornerMode(int axis, int mode)
{
    ZmotionResult result;
    result.success = false;
    result.errorCode = 0;
    result.errorMessage = "";

    // 检查是否已经初始化
    if (!m_initialized) {
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查是否已经连接
    if (m_handle == nullptr) {
        result.errorCode = -2;
        result.errorMessage = "控制器未连接";
        qCritical() << result.errorMessage;
        return result;
    }

    // 验证轴号在有效范围内
    if (axis < 0 || axis >= m_axisMaxCount) {
        result.errorCode = -3;
        result.errorMessage = QString("轴号 %1 超出有效范围 [0-%2]")
                                .arg(axis).arg(m_axisMaxCount - 1);
        qWarning() << result.errorMessage;
        return result;
    }

    // 调用ZAux_Direct_SetCornerMode API设置拐角模式
    int32_t ret = ZAux_Direct_SetCornerMode(m_handle, axis, mode);

    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置拐角模式失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
    } else {
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "设置拐角模式成功";
        ZINFO << result.errorMessage << "轴:" << axis << "模式值:" << mode;
        
        // 记录具体启用的模式功能
        QString enabledModes;
        if (mode & 2) enabledModes += "自动拐角减速 ";
        if (mode & 8) enabledModes += "自动小圆限速 ";
        if (mode & 32) enabledModes += "自动倒角设置";
        if (!enabledModes.isEmpty()) {
            ZINFO << "启用的模式功能:" << enabledModes.trimmed();
        }
    }

    return result;
}

/**
 * @brief ZmotionAPI::directMoveAbsolute 绝对直线插补运动
 * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
 * @param distances 绝对距离列表，与axes列表索引一一对应
 * @param speed 插补速度值，应用于主轴（第一个轴）
 * @param accel 插补加速度值，应用于主轴（第一个轴）
 * @param decel 插补减速度值，应用于主轴（第一个轴）
 * @return ZmotionResult 包含执行结果的结构体
 */
ZmotionResult ZmotionAPI::directMoveAbsolute(const QList<int>& axes, const QList<float>& distances, 
                                           float speed, float accel, float decel)
{
    ZmotionResult result;
    result.success = false;
    result.errorCode = 0;
    result.errorMessage = "";

    // 检查是否已经初始化
    if (!m_initialized) {
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查是否已经连接
    if (m_handle == nullptr) {
        result.errorCode = -2;
        result.errorMessage = "控制器未连接";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查参数有效性
    if (axes.isEmpty()) {
        result.errorCode = -3;
        result.errorMessage = "轴号列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }

    if (axes.size() != distances.size()) {
        result.errorCode = -4;
        result.errorMessage = "轴号列表和距离列表长度不匹配";
        qWarning() << result.errorMessage;
        return result;
    }

    // 验证轴号在有效范围内
    for (int axis : axes) {
        if (axis < 0 || axis >= m_axisMaxCount) {
            result.errorCode = -5;
            result.errorMessage = QString("轴号 %1 超出有效范围 [0-%2]")
                                    .arg(axis).arg(m_axisMaxCount - 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }

    // 获取主轴（第一个轴）
    int mainAxis = axes.first();
    int32_t ret = 0;

    // 设置插补速度
    ret = ZAux_Direct_SetSpeed(m_handle, mainAxis, speed);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补速度:" << speed;

    // 设置插补加速度
    ret = ZAux_Direct_SetAccel(m_handle, mainAxis, accel);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补加速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补加速度:" << accel;

    // 设置插补减速度
    ret = ZAux_Direct_SetDecel(m_handle, mainAxis, decel);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补减速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补减速度:" << decel;

    // 转换Qt容器为C数组
    int axisCount = axes.size();
    QVector<int> axisArray = axes.toVector();
    QVector<float> distanceArray = distances.toVector();

    // 调用ZAux_Direct_MoveAbs API执行绝对直线插补运动
    ret = ZAux_Direct_MoveAbs(m_handle, axisCount, axisArray.data(), distanceArray.data());

    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("绝对直线插补运动失败，错误码: %1")
                                .arg(getErrorDescription(ret));
        qCritical() << result.errorMessage;
    } else {
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "绝对直线插补运动成功";
        ZINFO << result.errorMessage << "轴列表:" << axes << "距离列表:" << distances
                << "速度:" << speed << "加速度:" << accel << "减速度:" << decel;
    }

    return result;
}

/**
 * @brief ZmotionAPI::directMoveAbsoluteSP 绝对直线插补运动-带SP
 * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
 * @param distances 绝对距离列表，与axes列表索引一一对应
 * @param startSpeed 开始速度值，应用于主轴（第一个轴）
 * @param endSpeed 结束速度值，应用于主轴（第一个轴）
 * @param forceSpeed 强制速度值，应用于主轴（第一个轴）
 * @param accel 插补加速度值，应用于主轴（第一个轴）
 * @param decel 插补减速度值，应用于主轴（第一个轴）
 * @return ZmotionResult 包含执行结果的结构体
 * @note 开始速度、结束速度、强制速度、加速度、减速度值必须在有效范围内，否则会导致运动失败
 */
ZmotionResult ZmotionAPI::directMoveAbsoluteSP(const QList<int> &axes, const QList<float> &distances, float startSpeed, float endSpeed, float forceSpeed, float accel, float decel)
{
    ZmotionResult result;

    // 检查是否已经初始化
    if (!m_initialized) {
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查是否已经连接
    if (m_handle == nullptr) {
        result.errorCode = -2;
        result.errorMessage = "控制器未连接";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查参数有效性
    if (axes.isEmpty()) {
        result.errorCode = -3;
        result.errorMessage = "轴号列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }

    if (axes.size() != distances.size()) {
        result.errorCode = -4;
        result.errorMessage = "轴号列表和距离列表长度不匹配";
        qWarning() << result.errorMessage;
        return result;
    }

    // 验证轴号在有效范围内
    for (int axis : axes) {
        if (axis < 0 || axis >= m_axisMaxCount) {
            result.errorCode = -5;
            result.errorMessage = QString("轴号 %1 超出有效范围 [0-%2]")
                                    .arg(axis).arg(m_axisMaxCount - 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }

    // 获取主轴（第一个轴）
    int mainAxis = axes.first();
    int32_t ret = 0;

    //设置运动开始速度
    result = setAxisParamValue(mainAxis,AxisParamsCommand::Startmove_Speed,startSpeed);
    if(result.success == false) {
        return result;
    }

    //设置强制运动速度
    result = setAxisParamValue(mainAxis,AxisParamsCommand::Force_Speed,forceSpeed);
    if(result.success == false) {
        return result;
    }

    //设置运动结束速度
    result = setAxisParamValue(mainAxis,AxisParamsCommand::Endmove_Speed,endSpeed);
    if(result.success == false) {
        return result;
    }

    // 设置插补加速度
    ret = ZAux_Direct_SetAccel(m_handle, mainAxis, accel);
    if(ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补加速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补加速度:" << accel;

    // 设置插补减速度
    ret = ZAux_Direct_SetDecel(m_handle, mainAxis, decel);
    if(ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补减速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补减速度:" << decel;

    // 转换Qt容器为C数组
    int axisCount = axes.size();
    QVector<int> axisArray = axes.toVector();
    QVector<float> distanceArray = distances.toVector();

    //启动绝对值插补运动-SP
    ret = ZAux_Direct_MoveAbsSp(m_handle, axisCount, axisArray.data(), distanceArray.data());
    if(ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("绝对值插补运动-SP失败，错误码: %1")
                                .arg(getErrorDescription(ret));
        qCritical() << result.errorMessage;
        return result;
    }
    
    result.success = true;
    result.errorCode = 0;
    result.errorMessage = "绝对值插补运动-SP成功";
    ZINFO << result.errorMessage << "轴列表:" << axes << "距离列表:" << distances
                << "开始速度:" << startSpeed << "结束速度:" << endSpeed << "强制速度:" << forceSpeed
                << "加速度:" << accel << "减速度:" << decel;

    return result;  
}

/**
 * @brief ZmotionAPI::directMultiMoveAbsolute 多条多轴直线绝对直线插补运动
 * @param axes 轴号列表（0开始），支持单个或多个轴，第一个轴为主轴
 * @param distancesList 多段运动的距离列表，每个子列表表示一段运动的距离，与axes列表索引一一对应
 * @param speed 插补速度值，应用于主轴（第一个轴）
 * @param accel 插补加速度值，应用于主轴（第一个轴）
 * @param decel 插补减速度值，应用于主轴（第一个轴）
 * @return ZmotionResult 包含执行状态、错误码和错误信息
 */
ZmotionResult ZmotionAPI::directMultiMoveAbsolute(const QList<int>& axes, const QList<QList<float>>& distancesList,
                                                float speed, float accel, float decel)
{
    ZmotionResult result;
    result.success = false;
    result.errorCode = 0;
    result.errorMessage = "";

    // 检查是否已经初始化
    if (!m_initialized) {
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查是否已经连接
    if (m_handle == nullptr) {
        result.errorCode = -2;
        result.errorMessage = "控制器未连接";
        qCritical() << result.errorMessage;
        return result;
    }

    // 检查参数有效性
    if (axes.isEmpty()) {
        result.errorCode = -3;
        result.errorMessage = "轴号列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }

    if (distancesList.isEmpty()) {
        result.errorCode = -4;
        result.errorMessage = "运动距离列表不能为空";
        qWarning() << result.errorMessage;
        return result;
    }

    // 验证每段运动的距离列表长度是否与轴号列表匹配
    int axisCount = axes.size();
    for (int i = 0; i < distancesList.size(); ++i) {
        if (distancesList.at(i).size() != axisCount) {
            result.errorCode = -5;
            result.errorMessage = QString("第 %1 段运动的距离列表长度与轴号列表不匹配")
                                    .arg(i + 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }

    // 验证轴号在有效范围内
    for (int axis : axes) {
        if (axis < 0 || axis >= m_axisMaxCount) {
            result.errorCode = -6;
            result.errorMessage = QString("轴号 %1 超出有效范围 [0-%2]")
                                    .arg(axis).arg(m_axisMaxCount - 1);
            qWarning() << result.errorMessage;
            return result;
        }
    }

    // 获取主轴（第一个轴）
    int mainAxis = axes.first();
    int32_t ret = 0;

    // 设置插补速度
    ret = ZAux_Direct_SetSpeed(m_handle, mainAxis, speed);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补速度:" << speed;

    // 设置插补加速度
    ret = ZAux_Direct_SetAccel(m_handle, mainAxis, accel);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补加速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补加速度:" << accel;

    // 设置插补减速度
    ret = ZAux_Direct_SetDecel(m_handle, mainAxis, decel);
    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("设置插补减速度失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
        return result;
    }
    ZINFO << "设置主轴" << mainAxis << "插补减速度:" << decel;

    // 转换Qt容器为C数组
    QVector<int> axisArray = axes.toVector();
    int moveCount = distancesList.size();
    
    // 创建一维距离数组，按照ZMC API要求的格式组织数据
    QVector<float> distanceArray;
    distanceArray.reserve(moveCount * axisCount);
    
    for (int i = 0; i < moveCount; ++i) {
        for (int j = 0; j < axisCount; ++j) {
            distanceArray.append(distancesList.at(i).at(j));
        }
    }

    // 调用ZAux_Direct_MultiMoveAbs API执行多条多轴绝对直线插补运动
    ret = ZAux_Direct_MultiMoveAbs(m_handle, moveCount, axisCount, axisArray.data(), distanceArray.data());

    if (ret != 0) {
        result.errorCode = ret;
        result.errorMessage = QString("多条多轴绝对直线插补运动失败，错误码: %1")
                                .arg(ret);
        qCritical() << result.errorMessage;
    } else {
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "多条多轴绝对直线插补运动成功";
        ZINFO << result.errorMessage << "轴列表:" << axes 
                << "运动段数:" << moveCount 
                << "速度:" << speed << "加速度:" << accel << "减速度:" << decel;
    }

    return result;
}

/*
 * @brief directMoveOp 设置输出插补指令
 * 
 * 该函数用于将指定的输出IO写入到运动缓冲中，在运动过程中按顺序执行IO操作。
 * 使用ZAux_Direct_MoveOp API实现。
 * 
 * @param axis 轴号（0开始），为主轴轴号
 * @param ioNumber IO编号
 * @param ioValue IO状态（0或1）
 * @return ZmotionResult 包含执行结果的结构体
 */
ZmotionResult ZmotionAPI::directMoveOp(int axis, int ioNumber, int ioValue)
{
    ZmotionResult result;
    
    // 检查初始化状态
    if (!m_initialized || m_handle == NULL) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 检查轴号范围
    if (axis < 0 || axis >= m_axisMaxCount) {
        result.success = false;
        result.errorCode = 218; // 参数错误
        result.errorMessage = QString("轴号超出范围(0-%1): %2").arg(m_axisMaxCount - 1).arg(axis);
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 检查IO状态值范围
    if (ioValue != 0 && ioValue != 1) {
        result.success = false;
        result.errorCode = 218; // 参数错误
        result.errorMessage = QString("IO状态值必须为0或1，当前值: %1").arg(ioValue);
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 调用ZAux_Direct_MoveOp API
    int ret = ZAux_Direct_MoveOp(m_handle, axis, ioNumber, ioValue);
    
    if (ret != 0) {
        // 调用失败
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("设置输出插补指令失败: %1").arg(getErrorDescription(ret));
        qCritical() << result.errorMessage << "轴号:" << axis << "IO编号:" << ioNumber << "IO状态:" << ioValue;
    } else {
        // 调用成功
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "设置输出插补指令成功";
        ZINFO << result.errorMessage << "轴号:" << axis << "IO编号:" << ioNumber << "IO状态:" << ioValue;
    }
    
    return result;
}

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
ZmotionResult ZmotionAPI::directMoveDelay(int axis, int delayMs)
{
    ZmotionResult result;
    
    // 检查初始化状态
    if (!m_initialized || m_handle == NULL) {
        qCritical() << "directMoveDelay: 控制器未初始化或未连接";
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        return result;
    }
    
    // 检查轴号范围（假设控制器最多支持32个轴）
    if (axis < 0 || axis > 31) {
        qCritical() << QString("directMoveDelay: 无效的轴号 %1，轴号范围应为0-31").arg(axis);
        result.success = false;
        result.errorCode = -2;
        result.errorMessage = QString("无效的轴号 %1，轴号范围应为0-31").arg(axis);
        return result;
    }
    
    // 检查延时时间是否有效
    if (delayMs < 0) {
        qCritical() << QString("directMoveDelay: 无效的延时时间 %1，延时时间不能为负数").arg(delayMs);
        result.success = false;
        result.errorCode = -3;
        result.errorMessage = QString("无效的延时时间 %1，延时时间不能为负数").arg(delayMs);
        return result;
    }
    
    // 调用ZAux_Direct_MoveDelay API
    int ret = ZAux_Direct_MoveDelay(m_handle, axis, delayMs);
    
    if (ret == 0) {
        ZINFO << QString("directMoveDelay: 轴%1 延时%2毫秒指令发送成功").arg(axis).arg(delayMs);
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "";
    } else {
        qCritical() << QString("directMoveDelay: 轴%1 延时%2毫秒指令发送失败，错误码: %3").arg(axis).arg(delayMs).arg(ret);
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("延时指令发送失败，错误码: %1").arg(ret);
    }
    
    return result;
}

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
ZmotionResult ZmotionAPI::setOp(int ioNumber, int ioValue)
{
    ZmotionResult result;
    
    // 参数验证
    if (ioNumber < 0) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = QString("无效的输出口编号 %1，输出口编号不能为负数").arg(ioNumber);
        return result;
    }
    
    if (ioValue != 0 && ioValue != 1) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = QString("无效的输出口状态值 %1，状态值只能为0或1").arg(ioValue);
        return result;
    }
    
    // 检查设备是否已连接
    if (!isConnected()) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "设备未连接或未初始化，无法执行输出口操作";
        return result;
    }
    
    // 调用ZAux_Direct_SetOp API
    int ret = ZAux_Direct_SetOp(m_handle, ioNumber, ioValue);
    
    if (ret == 0) {
        ZINFO << QString("setOp: 输出口%1 设置为%2状态成功").arg(ioNumber).arg(ioValue);
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "";
    } else {
        qCritical() << QString("setOp: 输出口%1 设置为%2状态失败，错误码: %3").arg(ioNumber).arg(ioValue).arg(ret);
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("设置输出口状态失败，错误码: %1").arg(ret);
    }
    
    return result;
}

/**
 * @brief getDI 获取单个输入口信号
 * 
 * 该函数用于获取单个输入口的状态，使用ZAux_Direct_GetIn API实现。
 * 
 * @param ioNumber 输入口编号
 * @return int 输入口状态值（0或1），-1表示获取失败
 * @note 输入口编号要大于控制器自身最大的IO编号，不能与控制器的编号重合
 * @note 使用示例：获取输入口1的状态
*/
int ZmotionAPI::getDI(int ioNumber)
{
    // 参数验证
    if (ioNumber < 0) {
        qWarning() << QString("getDI: 无效的输入口编号 %1，输入口编号不能为负数").arg(ioNumber);
        return -1;
    }
    
    // 检查设备是否已连接
    if (!isConnected()) {
        qWarning() << "getDI: 设备未连接或未初始化，无法执行输入口操作";
        return -1;
    }
    
    // ZAux_Direct_GetIn API获取单个输入口信号
    uint32 value = 0;
    int ret = ZAux_Direct_GetIn(m_handle, ioNumber, &value);
    
    if (ret == 0) {
        // ZINFO << QString("getDI: 获取输入口%1状态成功，值为%2").arg(ioNumber).arg(value);
        return value;
    } else {
        qCritical() << QString("getDI: 获取输入口%1状态失败，错误码: %2").arg(ioNumber).arg(ret);
        return -1;
    }
}

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
ZmotionResult ZmotionAPI::setInvertIn(int ioNumber, int invert)
{
    // 参数验证
    if (ioNumber < 0) {
        qWarning() << QString("setInvertIn: 无效的IO口编号 %1，IO口编号不能为负数").arg(ioNumber);
        return ZmotionResult(false, -1, QString("无效的IO口编号 %1").arg(ioNumber));
    }
    
    if (invert != 0 && invert != 1) {
        qWarning() << QString("setInvertIn: 无效的反转状态 %1，只能是0或1").arg(invert);
        return ZmotionResult(false, -1, QString("无效的反转状态 %1，只能是0或1").arg(invert));
    }
    
    // 检查设备是否已连接
    if (!isConnected()) {
        qWarning() << "setInvertIn: 设备未连接或未初始化，无法执行IO操作";
        return ZmotionResult(false, -1, "设备未连接或未初始化");
    }
    
    // 调用ZAux_Direct_SetInvertIn API设置输入IO反转
    int ret = ZAux_Direct_SetInvertIn(m_handle, ioNumber, invert);
    
    if (ret == 0) {
        ZINFO << QString("setInvertIn: 设置IO口%1反转状态为%2成功").arg(ioNumber).arg(invert);
        return ZmotionResult(true, 0, QString("设置IO口%1反转状态为%2成功").arg(ioNumber).arg(invert));
    } else {
        QString errorDesc = getErrorDescription(ret);
        qCritical() << QString("setInvertIn: 设置IO口%1反转状态失败，错误码: %2，错误描述: %3").arg(ioNumber).arg(ret).arg(errorDesc);
        return ZmotionResult(false, ret, QString("设置IO口%1反转状态失败: %2").arg(ioNumber).arg(errorDesc));
    }
}

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
ZmotionResult ZmotionAPI::getInMulti(int startIo, int endIo)
{
    ZmotionResult result;
    
    // 参数验证
    if (startIo < 0) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = QString("无效的起始IO口编号 %1，IO口编号不能为负数").arg(startIo);
        return result;
    }
    
    if (endIo < startIo) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = QString("无效的IO口范围，结束编号%1必须大于或等于起始编号%2").arg(endIo).arg(startIo);
        return result;
    }
    
    // 检查设备是否已连接
    if (!isConnected()) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "设备未连接或未初始化，无法执行输入口操作";
        return result;
    }
    
    // 调用ZAux_Direct_GetInMulti API获取多路输入口信号
    int32_t multiValue = 0;
    int ret = ZAux_Direct_GetInMulti(m_handle, startIo, endIo, &multiValue);
    
    if (ret == 0) {
        // ZINFO << QString("getInMulti: 获取IO口%1-%2状态成功，值为%3").arg(startIo).arg(endIo).arg(multiValue);
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "";
        result.piMultiValue = multiValue;
    } else {
        qCritical() << QString("getInMulti: 获取IO口%1-%2状态失败，错误码: %3").arg(startIo).arg(endIo).arg(ret);
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("获取多路输入口状态失败，错误码: %1").arg(ret);
    }
    
    return result;
}


/**
 * @brief setMoveOp 设置输出插补指令
 * 
 * 该函数用于将指定的输出IO写入到运动缓冲中，在运动过程中按顺序执行IO操作。
 * 使用ZAux_Direct_MoveOp API实现。
 * 
 * @param axis 轴号（0开始），为主轴轴号
 * @param ioNumber IO编号
 * @param ioValue IO状态（0或1）
 * @return ZmotionResult 包含执行结果的结构体
 */
ZmotionResult ZmotionAPI::setMoveOp(int axis, int ioNumber, int ioValue)
{
    ZmotionResult result;

    // 检查初始化状态
    if (!m_initialized || m_handle == NULL) {
        qCritical() << "setMoveDelay: 控制器未初始化或未连接";
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        return result;
    }



    //参数验证
    if (axis < 0 || axis >= m_axisMaxCount) {
        result.success = false;
        result.errorCode = 218; // 参数错误
        result.errorMessage = QString("轴号超出范围(0-%1): %2").arg(m_axisMaxCount - 1).arg(axis);
        qCritical() << result.errorMessage;
        return result;
    }
    
    //参数验证
    if (ioValue != 0 && ioValue != 1) {
        result.success = false;
        result.errorCode = 218; // 参数错误
        result.errorMessage = QString("IO状态值必须为0或1，当前值: %1").arg(ioValue);
        qCritical() << result.errorMessage;
        return result;
    }
    
    // 调用ZAux_Direct_MoveOp API
    int ret = ZAux_Direct_MoveOp(m_handle, axis, ioNumber, ioValue);
    
    if (ret != 0) {
        // 调用失败
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("设置输出插补指令失败: %1").arg(getErrorDescription(ret));
        qCritical() << result.errorMessage << "轴号:" << axis << "IO编号:" << ioNumber << "IO状态:" << ioValue;
    } else {
        // 调用成功
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "设置输出插补指令成功";
        ZINFO << result.errorMessage << "轴号:" << axis << "IO编号:" << ioNumber << "IO状态:" << ioValue;
    }
    
    return result;
}


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
ZmotionResult ZmotionAPI::setMoveDelay(int axis, int delayMs)
{
    ZmotionResult result;
    
    // 检查初始化状态
    if (!m_initialized || m_handle == NULL) {
        qCritical() << "setMoveDelay: 控制器未初始化或未连接";
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未初始化或未连接";
        return result;
    }
    //验证参数
    if (axis < 0 || axis >= m_axisMaxCount) {
        result.success = false;
        result.errorCode = 218; // 参数错误
        result.errorMessage = QString("轴号超出范围(0-%1): %2").arg(m_axisMaxCount - 1).arg(axis);
        qCritical() << result.errorMessage;
        return result;
    }

    //执行指令
    int ret = ZAux_Direct_MoveDelay(m_handle, axis, delayMs);
    
    if (ret != 0) {
        // 调用失败
        result.success = false;
        result.errorCode = ret;
        result.errorMessage = QString("设置延时插补指令失败: %1").arg(getErrorDescription(ret));
        qCritical() << result.errorMessage << "轴号:" << axis << "延时时间:" << delayMs;
    } else {
        // 调用成功
        result.success = true;
        result.errorCode = 0;
        result.errorMessage = "设置延时插补指令成功";
        ZINFO << result.errorMessage << "轴号:" << axis << "延时时间:" << delayMs;
    }
    
    return result;
}

/**
 * @brief waitForAxesIdle 等待指定轴停止运动
 * 
 * 等待指定的轴列表停止运动，最多等待指定的超时时间。
 * 使用 ZAux_Direct_GetParam 读取轴状态参数来判断是否正在运动。
 * 
 * @param axes 轴号列表
 * @param timeoutMs 超时时间（毫秒）
 * @return ZmotionResult 执行结果，success表示是否在超时前所有轴都已停止
 */
ZmotionResult ZmotionAPI::waitForAxesIdle(const QList<int>& axes, int timeoutMs)
{
    ZmotionResult result;
    
    if (!m_initialized || m_handle == NULL) {
        result.success = false;
        result.errorCode = -1;
        result.errorMessage = "控制器未连接";
        return result;
    }

    int elapsedMs = 0;
    const int checkIntervalMs = 50;

    while (elapsedMs < timeoutMs) {
        bool allIdle = true;

        for (int axis : axes) {
            int idleStatus = 0;
            // 优先用GetIfIdle2，不支持时回退到GetIfIdle
            int ret = ZAux_Direct_GetIfIdle2(m_handle, axis, &idleStatus);
            if (ret != ERR_SUCCESS) {
                ret = ZAux_Direct_GetIfIdle(m_handle, axis, &idleStatus);
            }
            if (ret == ERR_SUCCESS) {
                if (idleStatus == 0) {  // 0 = 运动中
                    allIdle = false;
                    break;
                }
            } else {
                // 两个API都不支持，保守认为轴在运动中，继续等待
                allIdle = false;
                break;
            }
        }

        if (allIdle) {
            result.success = true;
            result.errorCode = 0;
            result.errorMessage = QStringLiteral("所有轴已停止");
            return result;
        }

        QCoreApplication::processEvents();
        QThread::msleep(checkIntervalMs);
        elapsedMs += checkIntervalMs;
    }
    
    result.success = false;
    result.errorCode = -2;
    result.errorMessage = QString("等待轴停止超时 (%1ms)").arg(timeoutMs);
    return result;
}

/**
 * @brief waitForAxesReachTarget 等待轴到达目标位置
 *
 * 轮询对比当前 DPOS 与目标值的偏差，全部轴进入容差范围内即返回。
 * 比 waitForAxesIdle 更适合虚拟轴和 CONNFRAME 模式。
 *
 * @param axes      轴号列表
 * @param targets   目标位置列表，与 axes 一一对应
 * @param timeoutMs 超时时间
 * @param tolerance 位置容差
 */
ZmotionResult ZmotionAPI::waitForAxesReachTarget(const QList<int>& axes, const QList<float>& targets,
                                                  int timeoutMs, float tolerance)
{
    ZmotionResult result;
    if (!m_initialized || m_handle == NULL) {
        return {false, -1, QStringLiteral("控制器未连接")};
    }
    if (axes.size() != targets.size()) {
        return {false, -3, QStringLiteral("轴列表与目标列表长度不匹配")};
    }

    int elapsed = 0;
    const int interval = 30;  // 30ms 轮询

    while (elapsed < timeoutMs) {
        bool allReached = true;
        for (int i = 0; i < axes.size(); ++i) {
            float curPos = 0.0f;
            int ret = ZAux_Direct_GetDpos(m_handle, axes[i], &curPos);
            if (ret != ERR_SUCCESS) {
                // 读不到轴位置，保守等待
                allReached = false;
                break;
            }
            if (qAbs(curPos - targets[i]) > tolerance) {
                allReached = false;
                break;
            }
        }
        if (allReached) {
            return {true, 0, QStringLiteral("所有轴已到达目标")};
        }
        QCoreApplication::processEvents();
        QThread::msleep(interval);
        elapsed += interval;
    }
    return {false, -2, QString("等待轴到位超时 (%1ms)").arg(timeoutMs)};
}

/**
 * @brief getIfIdle2 判断轴是否真正停止（引入编码器速度判断）
 *
 * 比 ZAux_Direct_GetParam("IDLE") 更准确。
 * 当 IDLE=-1 且 MSPEED=0 时，轴实际已停止，此函数会正确返回停止状态。
 *
 * @param axis 轴号
 * @return ZmotionResult，axisHomeStatus: 0=运动中, 非0=已停止
 */
ZmotionResult ZmotionAPI::getIfIdle2(int axis)
{
    if (!m_initialized || m_handle == NULL) {
        return ZmotionResult(false, -1, "控制器未连接");
    }

    int idleStatus = 0;
    int ret = ZAux_Direct_GetIfIdle2(m_handle, axis, &idleStatus);

    if (ret == ERR_SUCCESS) {
        // 0 = 运动中，非0 = 已停止
        return ZmotionResult(true, 0, "", QStringList(), QList<int>(), 0, 0.0f, idleStatus);
    } else {
        return ZmotionResult(false, ret, getErrorDescription(ret));
    }
}

// 逆解模式 (CONNFRAME: virtual驱动→joint, IK)
ZmotionResult ZmotionAPI::startInverseKinematics(const QList<int>& jointAxes, const QList<int>& virtualAxes)
{
    if (!m_initialized || m_handle == NULL) {
        return {false, -1, QStringLiteral("控制器未连接")};
    }
    int count = qMin(jointAxes.size(), virtualAxes.size());
    QVector<int> jAxes(jointAxes.begin(), jointAxes.end());
    QVector<int> vAxes(virtualAxes.begin(), virtualAxes.end());
    waitForAxesIdle(jointAxes, 500);
    int ret = ZAux_Direct_Connframe(m_handle, count, jAxes.data(), kFrame, 0, count, vAxes.data());
    if (ret != ERR_SUCCESS) {
        return {false, ret, QString("Connframe失败: %1").arg(getErrorDescription(ret))};
    }
    m_currentMotionMode = 1;
    qDebug() << "[ZmotionAPI] 逆解模式(Connframe)启动成功"
             << "jointAxes:" << jointAxes << "virtualAxes:" << virtualAxes;
    return {true, 0, "逆解模式启动成功"};
}

// 正解模式 (CONNREFRAME: joint驱动→virtual, FK)
ZmotionResult ZmotionAPI::startForwardKinematics(const QList<int>& jointAxes, const QList<int>& virtualAxes)
{
    if (!m_initialized || m_handle == NULL) {
        return {false, -1, QStringLiteral("控制器未连接")};
    }
    int count = qMin(jointAxes.size(), virtualAxes.size());
    QVector<int> jAxes(jointAxes.begin(), jointAxes.end());
    QVector<int> vAxes(virtualAxes.begin(), virtualAxes.end());
    waitForAxesIdle(jointAxes, 500);
    int ret = ZAux_Direct_Connreframe(m_handle, count, vAxes.data(), kFrame, 0, count, jAxes.data());
    if (ret != ERR_SUCCESS) {
        return {false, ret, QString("Connreframe失败: %1").arg(getErrorDescription(ret))};
    }
    m_currentMotionMode = 0;
    return {true, 0, "正解模式启动成功"};
}

