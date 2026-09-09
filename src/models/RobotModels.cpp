/*
 * @file RobotModels.cpp
 * @brief RobotModelSpec / RobotModelRegistry 的实现，及 R01 机型参数定义。
 *
 * R01 即原 Robot_R06 的参数，齿轮链/速度/传感器/回零/IP 等归总，
 * 作为型号管理的唯一代码侧数据源；FRAME6 几何为固定结构，仍保留在 RobotKinematics 硬编码。
 */
#include "RobotModels.h"

namespace {

// 齿轮比 "1:4" -> 分母 4；解析失败返回 1
int gearDenominator(const QString& g)
{
    QStringList parts = g.split(QLatin1Char(':'));
    if (parts.size() == 2) {
        bool ok = false;
        int denom = parts.at(1).toInt(&ok);
        if (ok && denom > 0)
            return denom;
    }
    return 1;
}

// 提取机型公共默认字段：轴数 / 出厂原点位姿 / 关机安全位姿 / 回零后动作 / 默认 IP / 状态间隔
RobotModelSpec buildSharedDefaults()
{
    RobotModelSpec s;
    s.axisCount = 6;  // 轴数（6 关节）
    s.homePosition = QList<double>{84.999, 68.620, -51.944, 4.229, 71.000, 90.000};  // 出厂原点位姿(6轴关节角,°)：Flash 为空时作种子写入
    s.shutdownPosition = QList<double>{0, -50, 50, 0, 0, 0};  // 关机安全位姿(6轴关节角,°)：断电前摆到的姿态
    s.moveToHomeAfterHoming = true;  // 默认：回零成功后移动到 homePosition（具体见各机型 buildR01/buildR02）
    s.defaultControllerIP = QStringLiteral("192.168.0.123");  // 默认控制器 IP
    s.stateUpdateIntervalMs = 100;  // 状态更新周期(ms)
    return s;
}

/***********************************R01机型***************************************************/
RobotModelSpec buildR01()
{
    RobotModelSpec s = buildSharedDefaults();
    s.id = RobotModelId::R01;
    s.name = QStringLiteral("Robot_R01");
    s.homeMoveSpeed = 60;  // 回零后移动速度 (°/s)，R01 独立值
    s.homeMoveAccel = 80;  // 回零后移动加速度
    s.homeMoveDecel = 100; // 回零后移动减速度
    // ============ 各轴回零参数 HomeSpec：一行一轴 ============
    // J0~J5 依次填入 axes[0]~axes[5]，每行 11 个数字按序对应：
    //   1 homeIn          原点传感器 IO 号（-1=未接）
    //   2 negIn           反向限位传感器 IO 号（-1=未接）
    //   3 datumMode       回零模式：3=正向限位回零，4=反向限位回零
    //   4 moveAwayOrigin  开机时原点已被压住，先移开的距离(°)，负=反向移
    //   5 moveAwayReverse 开机时反向限位已被压住，先移开的距离(°)；无反限位时取 0
    //   6 homeSpeed       回零高速(°/s)，快速逼近传感器
    //   7 homeAccel       回零加速度(°/s²)
    //   8 homeDecel       回零减速度(°/s²)
    //   9 homeLspeed      起始低速(°/s)，防起步抖动，此处恒 0
    //  10 homeSramp       S 曲线加减速过渡时间(ms)
    //  11 homeCreep       压到原点后的低速爬行速度(°/s)，保证对点精度
    // 以 J0 行为例：{0,1,3,-10,-155,40,80,100,0,200,1}
    //   = 原点IO0 / 反限位IO1 / 模式3(正向回零) / 移开-10°、-155° /
    //     回零40°/s、加80、减100 / 起始0 / S曲线200ms / 爬行1°/s
    HomeSpec J0{0,    1,  3, -10,  -155, 40, 80,  100, 0, 200, 1};    
    HomeSpec J1{2,   -1,  4,  35,     0, 40, 80,  200, 0, 200, 1};  
    HomeSpec J2{3,   -1,  3, -35,     0, 20, 50,  100, 0, 200, 1};  
    HomeSpec J3{4,    5,  3, -10,  -200, 40, 80,  100, 0, 200, 1};  
    HomeSpec J4{6,   -1,  4,  20,     0, 40, 80,  100, 0, 200, 1};  
    HomeSpec J5{7,    8,  3, -10,  -220, 60, 100, 200, 0, 200, 2};  

    // 列：pulsePerRev, reductionRatio, gearRatio, home
    s.axes = QList<AxisSpec>{
        {1000, 50, QStringLiteral("1:4"), J0},  
        {1000, 50, QStringLiteral("1:4"), J1},  
        {1000, 30, QStringLiteral("1:1"), J2},  
        {1000, 50, QStringLiteral("1:1"), J3},  
        {1000, 30, QStringLiteral("1:1"), J4},  
        {1000, 10, QStringLiteral("1:1"), J5},  
    };
    return s;
}

/***********************************R02机型***************************************************/
RobotModelSpec buildR02()
{
    RobotModelSpec s = buildSharedDefaults();
    s.id = RobotModelId::R02;
    s.name = QStringLiteral("Robot_R02");
    s.homeMoveSpeed = 60;  // 回零后移动速度 (°/s)，R02 独立值
    s.homeMoveAccel = 80;  // 回零后移动加速度
    s.homeMoveDecel = 100; // 回零后移动减速度
    s.moveToHomeAfterHoming = true;  // R02：回零后移动到 homePosition（到位后统一清 DPOS=0）
    // TODO(临时): 用户指定临时原点数据（R02 原点偏移，待调整原点验证后移除或正式化）
    s.homePosition = QList<double>{-136.500, 25.000, -211.999, 9.381, -90.999, 0.000};
    // R02 关机安全位姿（与 R01 不同，J2 行程深，取 260°）
    s.shutdownPosition = QList<double>{0, -25, 260, 0, 0, 0};
    /* ============ 各轴回零参数 HomeSpec：一行一轴 ============
    // 以 J0 行为例：{0,1,3,-10,-155,40,80,100,0,200,1}
    //   = 原点IO0 / 反限位IO1 / 模式3(正向回零) / 移开-10°、-155° /
    //     回零40°/s、加80、减100 / 起始0 / S曲线200ms / 爬行1°/s*/
    HomeSpec J0{0,    1,  3, -10,  -155, 10, 80,  100, 0, 200, 1}; 
    HomeSpec J1{2,   -1,  4,  15,     0, 10, 80,  200, 0, 200, 1};  
    HomeSpec J2{3,   -1,  3, -35,     0, 10, 50,  100, 0, 200, 1}; 
    HomeSpec J3{4,    5,  3, -20,  -200, 10, 80,  100, 0, 200, 1};  
    HomeSpec J4{6,   -1,  3, -20,     0, 10, 80,  100, 0, 200, 1};  
    HomeSpec J5{7,    8,  3, -10,  -220, 10, 100, 200, 0, 200, 2};  

    // 列：pulsePerRev, reductionRatio, gearRatio, home
    // 电机脉冲为 10000，减速比固定为 28
    s.axes = QList<AxisSpec>{
        {10000, 28, QStringLiteral("1:4"), J0},  
        {10000, 28, QStringLiteral("1:4"), J1},  
        {10000, 28, QStringLiteral("1:1"), J2},  
        {10000, 28, QStringLiteral("1:1"), J3},  
        {10000, 28, QStringLiteral("1:1"), J4},  
        {10000, 28, QStringLiteral("1:1"), J5},  
    };
    return s;
}

// R01 规格单例（静态缓存，全程唯一）
const RobotModelSpec& r01Spec()
{
    static const RobotModelSpec s = buildR01();
    return s;
}

// R02 规格单例（静态缓存，全程唯一）
const RobotModelSpec& r02Spec()
{
    static const RobotModelSpec s = buildR02();
    return s;
}

// 空规格（R02/Unknown 占位，参数待填）
// 注：const 聚合对象必须显式初始化（值初始化），字段为 0/空，
//   但实际不会读这些字段 —— 见 RobotConfig::initializeFromModel 对 Unknown 的 R01 回退
const RobotModelSpec& emptySpec()
{
    static const RobotModelSpec s{};
    return s;
}

} // namespace

// 由齿轮链推导脉冲当量：pulsePerRev × reductionRatio × 齿轮比分母 / 360
QList<double> RobotModelSpec::computeAxisUnits() const
{
    QList<double> u;
    u.reserve(axes.size());
    for (const AxisSpec& a : axes) {
        double v = a.pulsePerRev * a.reductionRatio
                   * gearDenominator(a.gearRatio) / 360.0;
        u.append(v);
    }
    return u;
}

// 按机型 ID 取规格（R02/Unknown 返回空规格）
const RobotModelSpec& RobotModelRegistry::get(RobotModelId id)
{
    switch (id) {
    case RobotModelId::R01:
        return r01Spec();
    case RobotModelId::R02:
        return r02Spec();
    case RobotModelId::Unknown:
    default:
        return emptySpec();
    }
}

// 所有已知机型（用于 UI 下拉）
QList<RobotModelId> RobotModelRegistry::allModels()
{
    return QList<RobotModelId>{RobotModelId::R01, RobotModelId::R02};
}
