# Toggle Completions and IntelliSense Settings in VS Code
# This script toggles various settings to enable/disable completions and IntelliSense

$settingsPath = "$PSScriptRoot/../.vscode/settings.json"

# Check if settings.json exists
if (-not (Test-Path $settingsPath)) {
    Write-Host "settings.json not found. Creating a new one with completions disabled."
    $settings = @{
        "editor.tabCompletion" = "off"
        "editor.quickSuggestions" = @{
            "other" = "off"
            "comments" = "off"
            "strings" = "off"
        }
        "editor.suggestOnTriggerCharacters" = $false
        "editor.suggest.enabled" = $false
        "editor.parameterHints.enabled" = $false
        "editor.inlineSuggest.enabled" = $false
        "C_Cpp.autocomplete" = "disabled"
        "C_Cpp.intelliSenseEngine" = "disabled"
        "C_Cpp.codeAnalysis.runAutomatically" = $false
    }
    $settings | ConvertTo-Json -Depth 10 | Set-Content $settingsPath
    Write-Host "Completions and IntelliSense disabled."
    exit
}

# Read the current settings
$content = Get-Content $settingsPath -Raw
if ([string]::IsNullOrWhiteSpace($content)) {
    Write-Host "settings.json is empty. Creating with completions disabled."
    $settings = @{
        "editor.tabCompletion" = "off"
        "editor.quickSuggestions" = @{
            "other" = "off"
            "comments" = "off"
            "strings" = "off"
        }
        "editor.suggestOnTriggerCharacters" = $false
        "editor.suggest.enabled" = $false
        "editor.parameterHints.enabled" = $false
        "editor.inlineSuggest.enabled" = $false
        "C_Cpp.autocomplete" = "disabled"
        "C_Cpp.intelliSenseEngine" = "disabled"
        "C_Cpp.codeAnalysis.runAutomatically" = $false
    }
    $settings | ConvertTo-Json -Depth 10 | Set-Content $settingsPath
    Write-Host "Completions and IntelliSense disabled."
    exit
}

$settings = $content | ConvertFrom-Json

# Get current value for tabCompletion (null if not set)
$currentTabValue = $settings.'editor.tabCompletion'

# Determine if currently enabled or disabled
$isCurrentlyDisabled = ($currentTabValue -eq "off") -or ($currentTabValue -eq $null)

if ($isCurrentlyDisabled) {
    # Enable
    $newTabValue = "on"
    $newQuickValue = @{
        "other" = "on"
        "comments" = "on"
        "strings" = "on"
    }
    $newSuggestTrigger = $true
    $newSuggestEnabled = $true
    $newParamHints = $true
    $newInlineSuggest = $true
    $newAutocomplete = "default"
    $newIntelliSense = "default"
    $newCodeAnalysis = $true
    Write-Host "Completions and IntelliSense enabled."
} else {
    # Disable
    $newTabValue = "off"
    $newQuickValue = @{
        "other" = "off"
        "comments" = "off"
        "strings" = "off"
    }
    $newSuggestTrigger = $false
    $newSuggestEnabled = $false
    $newParamHints = $false
    $newInlineSuggest = $false
    $newAutocomplete = "disabled"
    $newIntelliSense = "disabled"
    $newCodeAnalysis = $false
    Write-Host "Completions and IntelliSense disabled."
}

# Set the new values using Add-Member
$settings | Add-Member -MemberType NoteProperty -Name 'editor.tabCompletion' -Value $newTabValue -Force
$settings | Add-Member -MemberType NoteProperty -Name 'editor.quickSuggestions' -Value $newQuickValue -Force
$settings | Add-Member -MemberType NoteProperty -Name 'editor.suggestOnTriggerCharacters' -Value $newSuggestTrigger -Force
$settings | Add-Member -MemberType NoteProperty -Name 'editor.suggest.enabled' -Value $newSuggestEnabled -Force
$settings | Add-Member -MemberType NoteProperty -Name 'editor.parameterHints.enabled' -Value $newParamHints -Force
$settings | Add-Member -MemberType NoteProperty -Name 'editor.inlineSuggest.enabled' -Value $newInlineSuggest -Force
$settings | Add-Member -MemberType NoteProperty -Name 'C_Cpp.autocomplete' -Value $newAutocomplete -Force
$settings | Add-Member -MemberType NoteProperty -Name 'C_Cpp.intelliSenseEngine' -Value $newIntelliSense -Force
$settings | Add-Member -MemberType NoteProperty -Name 'C_Cpp.codeAnalysis.runAutomatically' -Value $newCodeAnalysis -Force

# Save the updated settings
$settings | ConvertTo-Json -Depth 10 | Set-Content $settingsPath

Write-Host "Settings updated. Reload VS Code window to apply changes."
