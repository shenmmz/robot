/*
 * @file RobotModelId.h
 * @brief 机械臂型号枚举与名称映射（型号管理的数据标签）。
 *
 * 说明：
 * - 控制器 Flash 里只存这个枚举的整数值（小数字），不存全量参数。
 * - 完整参数（减速比/几何/回零/速度…）在代码侧的 RobotModelSpec 注册表里，
 *   由这个 ID 查表得到（见后续 RobotModels.h/.cpp）。
 * - 0 = Unknown 是哨兵，表示"未写入 / 读取失败"，不要当作真实机型。
 *
 * Flash 布局（型号记录，独立块，避免与原点块 0 冲突）：
 *   块 100, 4 个 float: [modelMagic, version, modelId, axisCount]
 */
#pragma once

#include <QString>

enum class RobotModelId : int {
    Unknown = 0,   // 哨兵：未写入 / 读取失败
    R01     = 1,   // 现有机械臂（减速比 50/30/10）
    R02     = 2,   // 新臂（减速比不同，参数待填）
    // 后续新增机型：在此往下排 3、4…
};

// ID -> 可读名称（用于日志 / UI 显示）
inline QString robotModelName(RobotModelId id)
{
    switch (id) {
    case RobotModelId::R01:     return QStringLiteral("R01");
    case RobotModelId::R02:     return QStringLiteral("R02");
    case RobotModelId::Unknown:
    default:                    return QStringLiteral("Unknown");
    }
}

// 名称 -> ID（用于 --setmodel 启动参数解析；大小写不敏感）
inline RobotModelId robotModelFromName(const QString& name)
{
    QString s = name.trimmed();
    if (s.compare(QStringLiteral("R01"), Qt::CaseInsensitive) == 0)
        return RobotModelId::R01;
    if (s.compare(QStringLiteral("R02"), Qt::CaseInsensitive) == 0)
        return RobotModelId::R02;
    return RobotModelId::Unknown;
}
