# 脉冲当量（axisUnits）数据链路重构设计

> 状态：设计已确认，待实施（2026-08-31）
> 范围：仅 `RobotConfig` 一个类（.h/.cpp），调用方与下游全部零改动

## 1. 背景与动机

`buildFrame6Table()` 里贴的 `axisUnits[0] * 360.0` 一行，其数据要经过 5 跳才到达：

```
RobotModels.cpp（字面数字 + computeAxisUnits 公式）
  → RobotConfig::initializeFromModel 缓存进 m_axisUnits      ← 第 1 跳：缓存
    → RobotConfig::getAxisUnitsList() 转发缓存               ← 第 2 跳：纯转发
      → RobotController.cpp:229 / HomingController.cpp:336   ← 第 3 跳：两个调用方
        → RobotKinematics::buildFrame6Table(axisUnits 参数)  ← 第 4/5 跳：形参
```

问题：
- `m_axisUnits` 缓存 + `getAxisUnitsList()` 纯转发是**多余的两跳**，不承载任何逻辑；
- IDE 的"跳转到定义"追踪不到跨函数数据流，`axisUnits` 是形参没有源头可跳，
  中间这层缓存转发让追代码的体验更差。

## 2. 关键事实（已逐一核实）

| 符号 | 位置 | 性质 |
|---|---|---|
| `axisUnits`（buildFrame6Table 内） | `src/models/RobotKinematics.cpp` | 形参，无数据源头 |
| `getAxisUnitsList()` 调用方 ① | `src/logic/RobotController.cpp:229` | 连接后 TABLE 建模 |
| `getAxisUnitsList()` 调用方 ② | `src/logic/HomingController.cpp:336` | `initAxisParams()` 回零时逐轴下发当量 |
| `m_axisUnits` 成员 | `RobotConfig.h:92`，赋值在 `RobotConfig.cpp:124` | 唯一持有处 |
| 字面数字 + 公式 | `src/models/RobotModels.cpp`（`buildR01()` 的 `s.axes` + `computeAxisUnits()`） | **真正的数据源** |

> ⚠️ 修正：早先"`getAxisUnitsList()` 全工程只有一个调用方"的说法是错的，
> 实际有 **两个**（RobotController + HomingController）。
> 因此不能直接删 getter——那需要改 HomingController 构造函数签名，改动面反而变大。

## 3. 方案对比

| | 路线①（删 getter，两调用方直查注册表） | **本设计（getter 实时推导）** | 维持现状 |
|---|---|---|---|
| 取数跳数 | 3 跳 | **3 跳** | 5 跳 |
| 改动文件数 | 3 个 | **2 个** | 0 |
| HomingController 构造函数 | 要改签名 | **不动** | — |
| 行为风险 | 中（改两条调用链） | **低（只换实现，接口不变）** | — |
| F12 跳转体验 | 直达 RobotModels.cpp | **同样直达** | 跳不动 |

## 4. 具体改动

### ① `src/models/RobotConfig.h`

- **删** L92 成员：`QList<double> m_axisUnits;`
- **加** 成员：`RobotModelId m_modelId = RobotModelId::Unknown;`
- `getAxisUnitsList()` 声明（L70）**不动**。

### ② `src/models/RobotConfig.cpp`（两处函数体）

**`initializeFromModel()` L124**

```cpp
// 改前
m_axisUnits = src.computeAxisUnits();

// 改后
m_modelId = src.id;
```

> 注意存的是**回退后**的 `src.id`：Unknown 机型回退 R01 后，存的就是 R01，语义正确。

**`getAxisUnitsList()` L68-70**

```cpp
// 改前
QList<double> RobotConfig::getAxisUnitsList() const
{
    return m_axisUnits;
}

// 改后
QList<double> RobotConfig::getAxisUnitsList() const
{
    return RobotModelRegistry::get(m_modelId).computeAxisUnits();
}
```

### ③ 零改动清单

`RobotController.cpp`、`HomingController.cpp`、`RobotKinematics`、`RobotModels` 全部不动。

## 5. 数据流（改后）

```
RobotModels.cpp（字面数字 + 公式，单一数据源）
  → RobotConfig::getAxisUnitsList()（实时调 registry.computeAxisUnits）
    → RobotController.cpp:229 / HomingController.cpp:336（不动）
      → buildFrame6Table(axisUnits)（不动）
```

## 6. 代价与不变量

- **代价**：每次调 `getAxisUnitsList()` 重算一次（解析 "1:4" 字符串 + 乘法）。
  调用频率 = 连接时 1 次 + 回零初始化 6 次，可忽略。
- **行为不变**：
  - 数值仍由 `RobotModels.cpp` 公式算出，R01 六轴当量保持
    555.56 / 555.56 / 83.33 / 138.89 / 83.33 / 27.78（脉冲/度）；
  - `initializeFromModel` 未调用时 getter 返回空列表（与现在 `m_axisUnits` 为空一致）；
  - "改减速比/齿轮比自动重算"能力保留（公式仍在 `computeAxisUnits()`）。
- **附加收益**：`RobotConfig` 从此持有 `m_modelId`，将来机型设置页可直接暴露
  `getModelId()`。

## 7. 验证方式

1. Ninja 全量构建 0 error / 0 warning；
2. 数值正确性由"公式与代码路径未变"保证（如需实证可在连接日志中核对六轴 UNITS 输出）。

## 8. 回退方式

改动只涉及 `RobotConfig.h/.cpp` 两个文件共 3 处，git 逆操作即可完整回退，
不影响其他文件。
