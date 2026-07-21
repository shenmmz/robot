# 同步 VisualRoboticArm 远端更新工作流

## 场景
`robot_arm` 分支上已有稳定运行的 VisualRoboticArm 代码，远端 `origin/visionproject` 有新的推理模型等更新，只需更新 `external/VisualRoboticArm/` 目录，不动其他项目代码。

## 步骤

### 1. 更新远端信息
```bash
git fetch origin
```

### 2. 只拉 VisualRoboticArm 目录
```bash
git checkout origin/visionproject -- external/VisualRoboticArm/
```

此命令只替换 `external/VisualRoboticArm/` 下的文件，其他所有项目代码（src/、mainwindow.*、CMakeLists.txt 等）完全不受影响。

### 3. 提交
```bash
git commit -m "sync: 从 visionproject 分支更新 VisualRoboticArm"
```

### 4. 检查新依赖并安装
对比 `requirements.txt` 的变更，在 venv 中安装新增的包：
```bash
external\VisualRoboticArm\.venv\Scripts\python.exe -m pip install <新包名>
```

## 注意事项
- `.venv` 在 `.gitignore` 中，Git 不会触碰虚拟环境及其已安装的包
- `__pycache__/` 等编译缓存不应被 Git 跟踪，合并时可能产生无关冲突，直接删除
- 此方式避免了 `git merge` 导致的全项目文件冲突（如 mainwindow.h、CMakeLists.txt 等）
- 如果只需验证而不想污染主分支，可在临时分支上操作，验证通过后再合入
