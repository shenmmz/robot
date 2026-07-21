param(
    [string]$OutputDir = "portable_package",
    [string]$Requirements = "requirements.txt",
    [string]$PythonExe = ".venv\Scripts\python.exe"
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

$pythonPath = Join-Path $repoRoot $PythonExe
if (-not (Test-Path $pythonPath)) {
    throw "Python executable was not found: $pythonPath. Run scripts\prepare_python_env.ps1 first."
}

$packageRoot = Join-Path $repoRoot $OutputDir
$wheelhouse = Join-Path $packageRoot "wheelhouse"
$scriptOut = Join-Path $packageRoot "scripts"

New-Item -ItemType Directory -Force -Path $wheelhouse | Out-Null
New-Item -ItemType Directory -Force -Path $scriptOut | Out-Null

Write-Host "Package directory: $packageRoot"
Write-Host "Downloading wheels to: $wheelhouse"

& $pythonPath -m pip download -r $Requirements -d $wheelhouse

Copy-Item -Path (Join-Path $repoRoot $Requirements) -Destination (Join-Path $packageRoot "requirements.txt") -Force
Copy-Item -Path (Join-Path $repoRoot "setup_env.bat") -Destination (Join-Path $packageRoot "setup_env.bat") -Force
Copy-Item -Path (Join-Path $repoRoot "scripts\prepare_python_env.ps1") -Destination (Join-Path $scriptOut "prepare_python_env.ps1") -Force

Write-Host ""
Write-Host "Portable dependency package generated."
Write-Host "After moving to the target computer, run this command from the project root:"
Write-Host "  .\scripts\prepare_python_env.ps1 -Offline -Wheelhouse .\$OutputDir\wheelhouse"
