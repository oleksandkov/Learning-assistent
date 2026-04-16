param(
    [switch]$Once,
    [int]$IntervalSeconds = 20
)

$ErrorActionPreference = 'SilentlyContinue'

function Test-ServerRunning([string]$RegexPattern) {
    $proc = Get-CimInstance Win32_Process |
        Where-Object { $_.CommandLine -and $_.CommandLine -match $RegexPattern } |
        Select-Object -First 1
    return [bool]$proc
}

function Start-ServerDetached([string]$Command) {
    Start-Process -FilePath "powershell" `
        -ArgumentList @("-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", $Command) `
        -WindowStyle Hidden | Out-Null
}

function Ensure-McpServers {
    # context7 (local stdio server process)
    if (-not (Test-ServerRunning 'context7-mcp')) {
        Start-ServerDetached 'npx -y @upstash/context7-mcp@latest'
    }

    # sequential thinking
    if (-not (Test-ServerRunning 'server-sequential-thinking')) {
        Start-ServerDetached 'npx -y @modelcontextprotocol/server-sequential-thinking'
    }

    # memory
    if (-not (Test-ServerRunning 'server-memory')) {
        Start-ServerDetached 'npx -y @modelcontextprotocol/server-memory'
    }

    # filesystem
    if (-not (Test-ServerRunning 'server-filesystem')) {
        Start-ServerDetached 'npx -y @modelcontextprotocol/server-filesystem "C:/Users/muaro/Documents/GitHub/Learning-assistent"'
    }

    # markitdown
    $uvxPath = 'C:\Users\muaro\AppData\Roaming\Python\Python313\Scripts\uvx.exe'
    if (-not (Test-ServerRunning 'markitdown-mcp')) {
        if (Test-Path $uvxPath) {
            Start-ServerDetached "& '$uvxPath' markitdown-mcp"
        } else {
            Start-ServerDetached 'uvx markitdown-mcp'
        }
    }

    # NOTE:
    # "pylance mcp server" shown in UI is usually managed by VS Code/Python tooling.
    # The npm package launcher is not reliable in this environment, so we intentionally
    # do not auto-launch it from this watchdog.
}

if ($Once) {
    Ensure-McpServers
    Write-Host 'MCP watchdog: one-shot ensure complete.'
    exit 0
}

Write-Host "MCP watchdog started (interval: $IntervalSeconds s)."
while ($true) {
    Ensure-McpServers
    Start-Sleep -Seconds $IntervalSeconds
}
