$ErrorActionPreference = 'SilentlyContinue'

$repo = git rev-parse --show-toplevel 2>$null
if (-not $repo) { $repo = (Get-Location).Path }

$watchdogScript = Join-Path $repo 'scripts/mcp_watchdog.ps1'

if (-not (Test-Path $watchdogScript)) {
    Write-Error "MCP watchdog script not found: $watchdogScript"
    exit 1
}

# one-shot ensure now
powershell -NoProfile -ExecutionPolicy Bypass -File $watchdogScript -Once

# keep-it-running daemon (start once)
$isWatchdogRunning = Get-CimInstance Win32_Process |
    Where-Object { $_.CommandLine -and $_.CommandLine -match 'scripts[\\/]mcp_watchdog\.ps1' } |
    Select-Object -First 1

if (-not $isWatchdogRunning) {
    Start-Process -FilePath "powershell" `
        -ArgumentList @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $watchdogScript) `
        -WindowStyle Hidden | Out-Null
}

Write-Host 'MCP watchdog ensured.'
