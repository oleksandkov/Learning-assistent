param(
    [Parameter(Mandatory = $true)][string]$File,
    [switch]$BuildOnly
)

chcp 65001 | Out-Null

$dir = Split-Path -Parent $File
$base = [IO.Path]::GetFileNameWithoutExtension($File)

# Walk up to project root (CMakeLists.txt or src/ marker), else stay in file dir.
$root = $dir
while ($root) {
    if ((Test-Path (Join-Path $root 'CMakeLists.txt')) -or (Test-Path (Join-Path $root 'src'))) { break }
    $parent = Split-Path -Parent $root
    if (-not $parent -or $parent -eq $root) { $root = $dir; break }
    $root = $parent
}

# Include dirs that actually exist: file dir, root/src, root/include, dir/include.
$incDirs = @($dir)
foreach ($d in @((Join-Path $root 'src'), (Join-Path $root 'include'), (Join-Path $dir 'include'))) {
    if ((Test-Path $d) -and ($incDirs -notcontains $d)) { $incDirs += $d }
}
$incArgs = $incDirs | ForEach-Object { "-I$_" }

function Get-LibArgs([string[]]$Files) {
    $libs = @()
    if (Select-String -Path $Files -Pattern '#include\s*[<"]SFML/' -Quiet) {
        $libs += '-lsfml-graphics', '-lsfml-window', '-lsfml-system', '-lsfml-audio'
    }
    if (Select-String -Path $Files -Pattern '#include\s*[<"](winsock2|winsock)\.h' -Quiet) { $libs += '-lws2_32' }
    if (Select-String -Path $Files -Pattern '#include\s*[<"]iphlpapi\.h' -Quiet) { $libs += '-liphlpapi' }
    if (Select-String -Path $Files -Pattern '#include\s*[<"](mmsystem|timeapi)\.h' -Quiet) { $libs += '-lwinmm' }
    return $libs
}

function Invoke-Build([string[]]$Sources, [string]$OutExe) {
    $args = @('-std=c++20') + $incArgs + $Sources + @('-o', $OutExe) + (Get-LibArgs $Sources)
    $out = & g++ @args 2>&1
    return @{ Code = $LASTEXITCODE; Output = ($out | Out-String) }
}

$srcDir = Join-Path $root 'src'
$multi = (Test-Path $srcDir) -and ((Get-ChildItem -LiteralPath $srcDir -Filter *.cpp -Recurse | Measure-Object).Count -gt 1)

if ($multi) {
    $sources = Get-ChildItem -LiteralPath $srcDir -Filter *.cpp -Recurse | ForEach-Object { $_.FullName }
    $exe = Join-Path $root "$base.exe"
    Set-Location -LiteralPath $root
} else {
    $sources = @($File)
    $exe = Join-Path $dir "$base.exe"
    Set-Location -LiteralPath $dir
}

function Test-Ascii([string]$s) { return $s -notmatch '[^\x00-\x7F]' }
if (-not (Test-Ascii $exe)) {
    # ld cannot write non-ASCII paths: emit the exe to an ASCII temp dir, still run it from the project dir.
    $sum = [System.BitConverter]::ToString(
        [System.Security.Cryptography.SHA1]::Create().ComputeHash(
            [System.Text.Encoding]::UTF8.GetBytes($root + '|' + $base))).Replace('-', '').Substring(0, 12)
    $outDir = Join-Path ([System.IO.Path]::GetTempPath()) "cppbuild_$sum"
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
    $exe = Join-Path $outDir "$base.exe"
}

$r = Invoke-Build $sources $exe

if ($r.Code -ne 0 -and -not $multi -and ($r.Output -match 'undefined reference')) {
    # Entry file needs siblings next to it: retry with every .cpp in its folder.
    $sibs = @(Get-ChildItem -LiteralPath $dir -Filter *.cpp | ForEach-Object { $_.FullName })
    if ($sibs.Count -gt 1) {
        Write-Host "[info] Single-file link failed, retrying with $($sibs.Count) files from $dir" -ForegroundColor Yellow
        $r = Invoke-Build $sibs $exe
    }
}

if ($r.Code -ne 0) { $r.Output | Write-Host; exit $r.Code }
Write-Host "[OK] Built $exe" -ForegroundColor Green

if (-not $BuildOnly) { & $exe }
