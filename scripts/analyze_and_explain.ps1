param([string]$file)
$ext = [System.IO.Path]::GetExtension($file).ToLower()
if ($ext -eq '.c' -or $ext -eq '.cpp') { $comment = '//' } else { $comment = '#' }

# Read code
$code = Get-Content -Path $file
$totalLines = $code.Count
$commentLines = ($code | Where-Object { $_.Trim().StartsWith($comment) }).Count
$emptyLines = ($code | Where-Object { $_.Trim() -eq '' }).Count
# Simple function detection (C/C++/Python/JS/Java style)
$functionLines = ($code | Select-String -Pattern '(^|\s)(int|void|float|double|char|bool|def|function|public|private|protected|static)\s+\w+\s*\(').Count

# Build explanation
$explanation = "`n`n$comment --- Code Explanation ---"
$explanation += "`n$comment File extension: $ext"
$explanation += "`n$comment Total lines: $totalLines"
$explanation += "`n$comment Comment lines: $commentLines"
$explanation += "`n$comment Empty lines: $emptyLines"
$explanation += "`n$comment Functions detected: $functionLines"
$explanation += "`n$comment This file contains code in $ext. Add more details as needed."

Add-Content -Path $file -Value $explanation
