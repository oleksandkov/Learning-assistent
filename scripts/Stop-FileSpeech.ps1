[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$speechProcesses = Get-CimInstance Win32_Process |
    Where-Object {
        ($_.Name -in @('powershell.exe', 'pwsh.exe')) -and
        $_.CommandLine -like '*Invoke-FileSpeech.ps1*'
    }

if (-not $speechProcesses) {
    Write-Host 'No active speech task was found.'
    return
}

foreach ($process in $speechProcesses) {
    Stop-Process -Id $process.ProcessId -Force
}

Write-Host "Stopped $($speechProcesses.Count) speech task(s)."
