param(
    [string]$PythonExe = "python",
    [string]$EnvDir = ".venv",
    [switch]$Offline
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

Write-Host "创建虚拟环境: $EnvDir"
& $PythonExe -m venv $EnvDir

$venvPython = Join-Path $repoRoot $EnvDir
$venvPython = Join-Path $venvPython "Scripts\python.exe"
if (-not (Test-Path $venvPython)) {
    throw "未找到虚拟环境 Python: $venvPython"
}

Write-Host "升级 pip"
& $venvPython -m pip install --upgrade pip

if ($Offline) {
    if (-not (Test-Path "wheelhouse")) {
        throw "未找到 wheelhouse 目录，无法离线安装"
    }
    Write-Host "使用本地 wheelhouse 安装依赖"
    & $venvPython -m pip install --no-index --find-links wheelhouse -r requirements-portable.txt
} else {
    Write-Host "在线安装依赖"
    & $venvPython -m pip install -r requirements-portable.txt
}

Write-Host ""
Write-Host "环境准备完成。"
Write-Host "建议在 Qt 主控中把 Python 可执行文件设置为:"
Write-Host $venvPython
