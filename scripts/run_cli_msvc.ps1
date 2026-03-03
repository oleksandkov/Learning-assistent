param([string]$FilePath)

chcp 65001 | Out-Null

$dir  = Split-Path $FilePath -Parent
$file = Split-Path $FilePath -Leaf
$base = [System.IO.Path]::GetFileNameWithoutExtension($FilePath)

# --- Знайти Visual Studio через vswhere ---
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vswhere)) {
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "  vswhere.exe не знайдено." -ForegroundColor Red
    Write-Host "  Встановіть Visual Studio з компонентами:" -ForegroundColor Yellow
    Write-Host "   - Desktop development with C++" -ForegroundColor Yellow
    Write-Host "   - C++/CLI support for v143 build tools" -ForegroundColor Yellow
    Write-Host "==========================================" -ForegroundColor Red
    Read-Host "Натисніть Enter для закриття"
    exit 1
}

$vsPath = & $vswhere -latest -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath 2>$null

if (-not $vsPath) {
    Write-Host "MSVC toolchain не знайдений. Встановіть компонент 'C++/CLI support'." -ForegroundColor Red
    Read-Host "Натисніть Enter"
    exit 1
}

$vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"

if (-not (Test-Path $vcvars)) {
    Write-Host "vcvars64.bat не знайдений: $vcvars" -ForegroundColor Red
    Read-Host "Натисніть Enter"
    exit 1
}

# --- Зібрати та запустити у новому вікні CMD ---
$cmd  = "chcp 65001"
$cmd += " & cd /d `"$dir`""
$cmd += " & call `"$vcvars`""
$cmd += " && cl /clr /EHsc /utf-8 `"$file`" /link /SUBSYSTEM:WINDOWS"
$cmd += " && if exist `"$base.exe`" (`"$base.exe`") else (echo Збірка не вдалась)"
$cmd += " & pause"

Start-Process cmd.exe -ArgumentList "/k $cmd"
