param(
    [string]$PythonExe = "py",
    [string]$PythonVersion = "3.12",
    [string]$EnvDir = ".venv",
    [string]$Requirements = "requirements.txt",
    [string]$Wheelhouse = "wheelhouse",
    [switch]$Offline
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $repoRoot

function Invoke-ProjectPython {
    param([string[]]$Arguments)

    if ($PythonExe -ieq "py") {
        & $PythonExe "-$PythonVersion" @Arguments
    } else {
        & $PythonExe @Arguments
    }
}

Write-Host "Project root: $repoRoot"
Write-Host "Target Python: $PythonExe $PythonVersion"

Invoke-ProjectPython @("-c", "import sys; raise SystemExit(0 if sys.version_info[:2] == (3, 12) else 1)")
if ($LASTEXITCODE -ne 0) {
    throw "Python 3.12 is required. Install Python 3.12 and retry."
}

$venvPython = Join-Path $repoRoot $EnvDir
$venvPython = Join-Path $venvPython "Scripts\python.exe"

if (Test-Path $venvPython) {
    Write-Host "Virtual environment already exists: $EnvDir"
    & $venvPython -c "import sys; raise SystemExit(0 if sys.version_info[:2] == (3, 12) else 1)"
    if ($LASTEXITCODE -ne 0) {
        throw "The existing virtual environment is not Python 3.12. Remove $EnvDir and retry."
    }
} else {
    Write-Host "Creating virtual environment: $EnvDir"
    Invoke-ProjectPython @("-m", "venv", $EnvDir)
}

if (-not (Test-Path $venvPython)) {
    throw "Virtual environment Python was not found: $venvPython"
}

Write-Host "Upgrading pip"
& $venvPython -m pip install --upgrade pip

if ($Offline) {
    if (-not (Test-Path $Wheelhouse)) {
        throw "Offline wheelhouse directory was not found: $Wheelhouse"
    }
    Write-Host "Installing dependencies from offline wheelhouse: $Wheelhouse"
    & $venvPython -m pip install --no-index --find-links $Wheelhouse -r $Requirements
} else {
    Write-Host "Installing dependencies online from: $Requirements"
    & $venvPython -m pip install -r $Requirements
}

Write-Host ""
Write-Host "Environment is ready."
Write-Host "Python executable:"
Write-Host $venvPython
