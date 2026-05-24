[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, ParameterSetName = 'ReadFile')]
    [ValidateNotNullOrEmpty()]
    [string]$Path,

    [Parameter(Mandatory = $true, ParameterSetName = 'ListVoices')]
    [switch]$ListVoices,

    [Parameter(ParameterSetName = 'ReadFile')]
    [ValidateRange(200, 5000)]
    [int]$ChunkSize = 1200,

    [Parameter(ParameterSetName = 'ReadFile')]
    [ValidateRange(-10, 10)]
    [int]$Rate = 0
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Voice selection: uncomment exactly one line and keep the others commented.
# $SelectedVoiceName = 'Microsoft David Desktop'
# $SelectedVoiceName = 'Microsoft Zira Desktop'
$SelectedVoiceName = 'Microsoft Mark Desktop'

function Get-InstalledVoiceNames {
    param(
        [Parameter(Mandatory = $true)]
        [System.Speech.Synthesis.SpeechSynthesizer]$Speaker
    )

    @($Speaker.GetInstalledVoices() | ForEach-Object { $_.VoiceInfo.Name })
}

try {
    Add-Type -AssemblyName System.Speech
} catch {
    throw "This offline speech helper requires Windows Speech (System.Speech)."
}

function Get-TextChunks {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Text,

        [Parameter(Mandatory = $true)]
        [int]$MaxLength
    )

    $normalizedText = $Text -replace "\r\n", "`n"
    $paragraphs = $normalizedText -split "`n{2,}"

    foreach ($paragraph in $paragraphs) {
        if ([string]::IsNullOrWhiteSpace($paragraph)) {
            continue
        }

        $currentChunk = ''
        $words = $paragraph -split '\s+'

        foreach ($word in $words) {
            if ([string]::IsNullOrWhiteSpace($word)) {
                continue
            }

            $candidate = if ([string]::IsNullOrWhiteSpace($currentChunk)) {
                $word
            } else {
                "$currentChunk $word"
            }

            if ($candidate.Length -gt $MaxLength) {
                if (-not [string]::IsNullOrWhiteSpace($currentChunk)) {
                    $currentChunk
                }
                $currentChunk = $word
            } else {
                $currentChunk = $candidate
            }
        }

        if (-not [string]::IsNullOrWhiteSpace($currentChunk)) {
            $currentChunk
        }
    }
}

$speaker = New-Object System.Speech.Synthesis.SpeechSynthesizer
try {
    if ($ListVoices.IsPresent) {
        $speaker.GetInstalledVoices() |
            ForEach-Object {
                [PSCustomObject]@{
                    Name        = $_.VoiceInfo.Name
                    Culture     = $_.VoiceInfo.Culture.Name
                    Gender      = $_.VoiceInfo.Gender.ToString()
                    Age         = $_.VoiceInfo.Age.ToString()
                    Description = $_.VoiceInfo.Description
                }
            } |
            Format-Table -AutoSize
        return
    }

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "File not found: $Path"
    }

    $text = Get-Content -LiteralPath $Path -Raw -Encoding UTF8
    if ([string]::IsNullOrWhiteSpace($text)) {
        Write-Warning "The file is empty: $Path"
        return
    }

    $speaker.Rate = $Rate
    $speaker.Volume = 100

    if (-not [string]::IsNullOrWhiteSpace($SelectedVoiceName)) {
        $installedVoiceNames = Get-InstalledVoiceNames -Speaker $speaker
        if ($installedVoiceNames -contains $SelectedVoiceName) {
            $speaker.SelectVoice($SelectedVoiceName)
        } else {
            $availableVoices = $installedVoiceNames -join ', '
            Write-Warning "Voice '$SelectedVoiceName' is not installed. Available voices: $availableVoices. Using the default system voice instead."
        }
    }

    foreach ($chunk in Get-TextChunks -Text $text -MaxLength $ChunkSize) {
        $speaker.Speak($chunk)
    }
} finally {
    $speaker.Dispose()
}