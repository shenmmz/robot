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

// 提取机型公共默认字段：轴数 / 回零后动作 / 默认 IP / 状态间隔
RobotModelSpec buildSharedDefaults()
{
    RobotModelSpec s;
    s.axisCount = 6;  // 轴数（6 关节）
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
    s.homeMoveSpeed = 30;  // 回零后移动速度 (°/s)，R01 独立值
    s.homeMoveAccel = 80;  // 回零后移动加速度
    s.homeMoveDecel = 100; // 回零后移动减速度
    // —— 机型特有位姿（不可下沉到 buildSharedDefaults）——
    // 机械原点（2026-09-10 实测标定：手臂点动到机械原点姿态后读界面"机械原点"标签）
    // 注意：该标签 = Flash原点 + 当前DPOS，即手臂当前的绝对坐标，故可直接作为 Flash 原点值/Float 空时 seed-72.000
    s.homePosition = QList<double>{146.000, 55.000, -50.000, 6.499, 72.000, -90.000};
    s.shutdownPosition = QList<double>{0, -52, 79, 0, 0, 0};  // 关机安全位姿(6轴关节角,°)：断电前摆到的姿态
    /*原点传感器 反向限位传感器 回零模式 移开原点 移开反向限位 回零高速 回零加速度 回零减速度 起始低速 起始S曲线 压到原点后的低速爬行速度*/
    HomeSpec J0{0,    1,  3, -10,  -300, 40, 80,  100, 0, 200, 1};    
    HomeSpec J1{2,   -1,  4,  35,     0, 15, 80,  200, 0, 200, 1};  
    HomeSpec J2{3,   -1,  3, -35,     0, 15, 50,  100, 0, 200, 1};  
    HomeSpec J3{4,    5,  3, -10,  -200, 15, 80,  100, 0, 200, 1};  
    HomeSpec J4{6,   -1,  4,  10,     0, 20, 80,  100, 0, 200, 1};  
    HomeSpec J5{7,    8,  3, -10,  -220, 60, 100, 200, 0, 200, 2};  
    // 列：pulsePerRev, reductionRatio, gearRatio, home
    s.axes = QList<AxisSpec>{
        {10000, 50, QStringLiteral("1:4"), J0},  
        {10000, 50, QStringLiteral("1:4"), J1},  
        {10000, 50, QStringLiteral("1:1"), J2},  
        {10000, 50, QStringLiteral("1:1"), J3},  
        {10000, 30, QStringLiteral("1:1"), J4},  
        {10000, 10, QStringLiteral("1:1"), J5},  
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

    s.homePosition = QList<double>{-136.500, 25.000, -211.999, 9.381, -90.999, 0.000};  //原点位置
    s.shutdownPosition = QList<double>{0, -25, 260, 0, 0, 0};  // 关机安全位姿
    /*原点传感器 反向限位传感器 回零模式 移开原点 移开反向限位 回零高速 回零加速度 回零减速度 起始低速 起始S曲线 压到原点后的低速爬行速度*/
    HomeSpec J0{0,    1,  3, -10,  -155, 40, 80,  100, 0, 200, 1}; 
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
