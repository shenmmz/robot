# 项目总结和文件导航

> 本文件帮助你快速找到需要的信息

---

## 📍 你现在在哪里？

你是一个刚接手六轴机械臂项目的开发者，项目基于Qt和CMake。

**现状**：
- ✅ 框架搭建完成
- ✅ 文档齐全
- ✅ 代码结构清晰
- ⏳ 等待学习和开发

---

## 📚 文档导航

### 🎯 我想快速了解项目
→ 读这个文件的【核心架构总览】部分（下面有）

### 🚀 我想立即开始开发
→ 阅读 [`QUICK_START.md`](QUICK_START.md)

### 📖 我想深入理解架构
→ 阅读 [`FRAMEWORK_GUIDE.md`](FRAMEWORK_GUIDE.md)

### 🛠️ 我不知道CMakeLists该怎么改
→ 阅读 [`CMAKE_GUIDE.md`](CMAKE_GUIDE.md)

### 📅 我想知道项目的发展方向
→ 阅读 [`ROADMAP.md`](ROADMAP.md)

### 🔍 我想看源代码
→ 进入 [`src/`](src/) 目录

### ⚙️ 我想修改机械臂配置
→ 编辑 [`config/robot_config.json`](config/robot_config.json)

---

## 🏗️ 核心架构总览

```
应用有5层，从上到下：

第1层 🖥️ UI层
    ├─ 负责：显示界面、接收用户输入
    ├─ 文件：src/ui/MainWindowNew.h/cpp
    └─ 职责：不处理复杂业务逻辑，只调用RobotController

第2层 🧠 业务逻辑层
    ├─ 负责：协调各模块、实现核心功能
    ├─ 文件：src/logic/RobotController.h/cpp
    └─ 职责：决策、业务流程、错误处理

第3层 📊 数据模型层
    ├─ 负责：存储状态和配置
    ├─ 文件：src/models/RobotState.h/cpp
    │        src/models/RobotConfig.h/cpp
    └─ 职责：数据存储、数据访问

第4层 🔧 硬件驱动层
    ├─ 负责：与硬件通信
    ├─ 文件：src/hardware/MotionController.h/cpp
    └─ 职责：参数检查、硬件命令、状态读取

第5层 🔌 硬件API层
    ├─ 负责：最底层的硬件通信
    ├─ 文件：ZmotionAPI/ZmotionAPI.h/cpp
    └─ 职责：实际的网络通信、协议处理

工具层 🛠️ 工具层（跨越所有层）
    ├─ 负责：日志、配置管理等
    └─ 文件：src/utils/Logger.h/cpp
```

---

## 🔄 数据流向示例

**场景：用户点击"移动轴0"按钮**

```
1️⃣ UI层
   用户点击按钮
   ↓
   MainWindowNew::onAxisSliderMoved()
   ↓
   调用 m_robotController->moveAxis(0, 45, ...)

2️⃣ 业务逻辑层
   RobotController::moveAxis()
   ↓
   检查前置条件
   调用 m_motionController->moveAxisAbsolute(...)
   ↓
   发送信号 operationStarted()

3️⃣ 硬件驱动层
   MotionController::moveAxisAbsolute()
   ↓
   检查参数（限位、速度等）
   调用 m_zmotionAPI->startAxisSingleMoveAbs(...)
   ↓
   更新 m_state->setStatus()
   发送信号 motionStarted()

4️⃣ 硬件API层
   ZmotionAPI::startAxisSingleMoveAbs()
   ↓
   发送网络命令到硬件

5️⃣ 反馈
   硬件接收命令
   ↓
   定期读取状态
   ↓
   RobotState 更新位置信息
   ↓
   信号 axisPositionChanged() 发出
   ↓
   UI 更新显示
```

---

## 📁 核心文件快速参考

| 文件 | 用途 | 何时修改 |
|------|------|---------|
| `src/models/RobotState.h/cpp` | 存储机械臂实时状态 | 需要新增状态字段 |
| `src/models/RobotConfig.h/cpp` | 存储机械臂配置参数 | 需要新增配置项 |
| `src/logic/RobotController.h/cpp` | 核心业务逻辑 | 添加新功能 |
| `src/hardware/MotionController.h/cpp` | 硬件通信 | 修改硬件接口 |
| `src/ui/MainWindowNew.h/cpp` | 用户界面 | 修改UI和交互 |
| `src/utils/Logger.h/cpp` | 日志系统 | 添加日志输出 |
| `config/robot_config.json` | 机械臂配置 | 修改速度、限位等参数 |
| `ZmotionAPI/ZmotionAPI.h/cpp` | 硬件API包装 | 极少修改 |

---

## ✨ 核心概念速查

### 信号槽（Qt 信号系统）
```cpp
// 在业务逻辑层定义信号
signals:
    void motionCompleted(int axisIndex);

// 在UI层连接信号
connect(m_robotController, &RobotController::motionCompleted,
        this, &MainWindowNew::onMotionCompleted);

// 在业务逻辑层发出信号
emit motionCompleted(0);
```

### 状态管理
```cpp
// 查询状态
if (m_state->getStatus() == RobotState::StatusMoving) {
    // 正在运动
}

// 修改状态（只在硬件控制层）
m_state->setStatus(RobotState::StatusIdle);
```

### 分层原则
```
❌ 错误：跨层访问
MainWindow -> ZmotionAPI  // 不要这样做！

✅ 正确：按层调用
MainWindow -> RobotController -> MotionController -> ZmotionAPI
```

---

## 🎓 学习路径（推荐）

### Week 1：理解
- [ ] 阅读 `FRAMEWORK_GUIDE.md`
- [ ] 理解分层架构
- [ ] 研究现有代码

### Week 2：实践
- [ ] 修改现有代码
- [ ] 添加简单功能（如新按钮）
- [ ] 学会使用调试器

### Week 3-4：开发
- [ ] 实现硬件通信
- [ ] 完成核心功能
- [ ] 编写错误处理

### Week 5+：优化
- [ ] 性能优化
- [ ] 代码重构
- [ ] 文档完善

---

## 🐛 常见问题速答

### Q: 编译失败，提示"undefined reference"？
A: CMakeLists.txt 中少了源文件。见 `CMAKE_GUIDE.md`。

### Q: 不知道该修改哪个文件？
A: 按照表格找到对应文件，文件中有详细注释。

### Q: 硬件连接失败？
A: 检查 `config/robot_config.json` 中的 IP 和端口。

### Q: 想添加新功能？
A: 见 `QUICK_START.md` 中的"常见任务"部分。

### Q: 代码能编译但运行时崩溃？
A: 使用调试器设置断点，查看日志输出（Logger）。

---

## 📋 项目检查清单

### 开发前
- [ ] 已阅读所有文档
- [ ] 理解分层架构
- [ ] 能编译项目
- [ ] 能运行项目

### 开发中
- [ ] 遵循分层架构
- [ ] 添加适当注释
- [ ] 使用日志输出
- [ ] 更新CMakeLists.txt

### 开发后
- [ ] 能编译成功
- [ ] 没有链接错误
- [ ] 功能正常工作
- [ ] 代码有注释
- [ ] 文档已更新

---

## 🚀 立即开始的3个步骤

### 1. 编译项目
```bash
cd Robot_R06
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### 2. 理解代码
```
打开 src/ 目录
阅读各文件的注释
运行项目，观察输出
```

### 3. 修改代码
```
选择一个简单的功能（如新按钮）
按照 QUICK_START.md 修改
重新编译并测试
```

---

## 💬 获取帮助

1. **查看注释**：所有关键代码都有注释
2. **查看日志**：运行时查看 Logger 输出
3. **查看文档**：
   - QUICK_START.md - 快速入门
   - FRAMEWORK_GUIDE.md - 深入理解
   - CMAKE_GUIDE.md - CMake 配置
   - ROADMAP.md - 发展方向
4. **调试代码**：使用调试器设置断点

---

## 📊 项目统计

- **源代码文件**: ~15 个
- **代码行数**: ~2000+ 行
- **文档**: 4 个详细指南
- **层级**: 5 层分层架构
- **功能**: 机械臂控制完整框架

---

## 🎯 最终建议

> 不要被复杂性吓倒，一步步来

1. **先读文档**：理解全局
2. **再看代码**：理解细节
3. **再动手做**：学会实践
4. **再优化改**：追求完美

**记住**：
- 每个文件都有注释，能帮你理解
- 每个函数都有说明，知道要做什么
- 每个类都有用途，明白各自职责
- 分层架构让代码更好理解和维护

---

## 📞 联系方式

- 项目文档：见本目录下的 .md 文件
- 代码注释：在每个 .h 和 .cpp 文件中
- 配置文件：`config/robot_config.json`

---

**祝你学习和开发顺利！加油！** 🚀

如有任何问题，先查阅相应的文档，答案通常在那里。

