# VMControl 模块使用说明

`VisionController` 是对 VisionMaster SDK 的 Qt Widgets 封装模块。示例程序中的 `MainWindow` 只作为功能演示层，实际方案加载、流程运行、模块参数页打开、数据解析和超时处理都由 `VMControl` 完成。

## 模块文件

- `vmtypes.h`：对外数据类型，包含命令枚举、状态枚举、回传事件、点位数据结构。
- `vmbase.h/.cpp`：底层 SDK 封装，直接调用 VisionMaster 方案、流程和模块接口。
- `vmcontrol.h/.cpp`：模块对外入口，使用统一命令槽接收操作，使用信号回传结果。

## 推荐接入

```cpp
#include "VisionController/vmcontrol.h"

auto *vmControl = new VMControl(this);

connect(this, &YourWindow::requestSolutionCommand,
        vmControl, &VMControl::executeSolutionCommand);
connect(this, &YourWindow::requestProcedureCommand,
        vmControl, &VMControl::executeProcedureCommand);
connect(this, &YourWindow::requestModuleCommand,
        vmControl, &VMControl::executeModuleCommand);

connect(vmControl, &VMControl::statusChanged,
        this, &YourWindow::onVmStatusChanged);
connect(vmControl, &VMControl::dataReady,
        this, &YourWindow::onVmDataReady);
connect(vmControl, &VMControl::messageReady,
        this, &YourWindow::onVmMessageReady);
```

## 命令入口

- `executeSolutionCommand(SolMotionType, QVariantMap)`：统一处理方案命令。`Close`、`Save`、`Run`、`Runing`、`Stop` 可只发送操作类型；`Load`、`SaveAs`、`Switching` 通过 `params["solPath"]` 和 `params["password"]` 传递数据。
- `executeProcedureCommand(ProcMotionType, processName)`：统一处理流程命令，由 VMControl 根据 `Run`、`Runing`、`Stop`、`Set` 分发。
- `executeModuleCommand(ModuMotionType, QVariantMap)`：统一处理模块参数页命令，仅保留 `Set` 与 `GroupSet`。`Set` 通过 `params["processName"]`、`params["moduleName"]` 打开流程模块；`GroupSet` 额外通过 `params["groupName"]` 打开组合模块内的模块。

## 输出信号

- `statusChanged(const VisionStatusEvent&)`：方案加载状态。
- `messageReady(const VisionMessageEvent&)`：运行日志。
- `dataReady(const VisionDataEvent&)`：流程列表、渲染句柄、参数句柄、点位数据或超时信息。

复制 `VisionController` 目录到其他 Qt 程序时，不需要引入 EventBus；上层界面只需要发送统一命令并消费上述三个输出信号。
