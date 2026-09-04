---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: 7fd4f4eb60674eb6db7e422f0766d751_e830fa67a77411f1ac80525400aeaaa3
    ReservedCode1: 7sM6qmu35sNHYFUCJ7h7M3h+3vn1k03VGoMXYiIdMUKcrSs3GDMQLj+kBHUHCXPGnBNFrBhWewM6wHj7T5HCG+5r/h8gHzfHxitihEJGsd7N0mbyN+JgLvMeHA/wDJtiai/ZcNgZ8EEpY3HDE5dZ47A9irQxxjlvyjjnXPjNupGJdG+7+ezmEkkm67k=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: 7fd4f4eb60674eb6db7e422f0766d751_e830fa67a77411f1ac80525400aeaaa3
    ReservedCode2: 7sM6qmu35sNHYFUCJ7h7M3h+3vn1k03VGoMXYiIdMUKcrSs3GDMQLj+kBHUHCXPGnBNFrBhWewM6wHj7T5HCG+5r/h8gHzfHxitihEJGsd7N0mbyN+JgLvMeHA/wDJtiai/ZcNgZ8EEpY3HDE5dZ47A9irQxxjlvyjjnXPjNupGJdG+7+ezmEkkm67k=
---

# Robot_R06 编译构建运行（最简版）

```powershell
cd C:\Users\111\Desktop\Robot_R06

# 1. 首次 / 删了 build 后执行一次（配置）
cmake -S . -B build -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/msvc2022_64

# 2. 编译（日常只跑这行）
cmake --build build --config Release --parallel 8

# 3. 运行
.\build\Release\Robot_R06.exe
```

- Debug 版：`Release` 改 `Debug`，运行 `.\build\Debug\Robot_R06.exe`
- 编译报 `not a directory`：先跑第 1 行配置
- Qt Creator 方式：打开 CMakeLists.txt → 选 Kit「Desktop Qt 6.9.0 MSVC2022 64bit」→ Ctrl+B 编译 → Ctrl+R 运行

## 常见链接错误：exe 被占用

**现象**：编译到链接阶段报
- `LNK1104: 无法打开文件 ...\Robot_R06.exe`（输出文件写不进去）
- 或 `LNK1168: 无法打开 ...\Robot_R06.exe 进行写入`

**原因**：之前运行过的 Robot_R06.exe 实例没有退出（可能残留多个后台实例），exe 文件被进程锁住，链接器无法覆盖。

**解决步骤**：

```powershell
# 1. 查看是否有实例在跑（无输出=没有进程）
Get-Process -Name Robot_R06

# 2. 结束全部实例（关键步骤）
Stop-Process -Name Robot_R06 -Force

# 3. 若还提示被占用，改用 taskkill 强杀
taskkill /F /IM Robot_R06.exe

# 4. 重新编译
cmake --build build --config Release --parallel 8
```

**预防**：每次运行完程序先关闭窗口，再改代码编译；或者编译前先跑一次 `Stop-Process -Name Robot_R06 -Force`。
*（内容由AI生成，仅供参考）*
*（内容由AI生成，仅供参考）*
