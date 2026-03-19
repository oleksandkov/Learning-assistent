@echo off
cd /d %~dp0
:: ========================================================================
:: UNIVERSAL PROGRAM LAUNCHER - Animated Loader
:: Usage: Set PROGRAM_EXE and TITLE below
:: ========================================================================

:: === USER CONFIGURATION ===
set "PROGRAM_EXE=your_program.exe"
set "TITLE=Program Launcher"
set "AUTHOR=your_name"
set "VERSION=1.0"
:: ==========================

setlocal EnableDelayedExpansion
color 0A
set "CONSOLE_TITLE=%TITLE%"
title !CONSOLE_TITLE!

:: Set console size (optional, can adjust as needed)
mode con: cols=100 lines=30

:: Carriage return for progress bar
for /f %%A in ('"prompt $H &echo on &for %%B in (1) do rem"') do set "BS=%%A"
set "CR=!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!"

:: Clear screen and show header
cls
echo.
echo    ╔══════════════════════════════════════════════════════════════════════════════╗
call :center_box "!TITLE!"
call :center_box "Author: !AUTHOR!    Version: !VERSION!"
echo    ╚══════════════════════════════════════════════════════════════════════════════╝

goto :skip_box







:center_box
setlocal EnableDelayedExpansion
set "str=%~1"
if "!str!"=="" goto :empty_center_box
set "box_width=78"
set "padstr=                                                                             "
:: Trim leading/trailing spaces
for /f "tokens=*" %%a in ("!str!") do set "str=%%a"
set "str=!str:~0,76!"
:: Calculate string length (ASCII-safe)
set "len=0"
for /l %%i in (0,1,77) do if not "!str:~%%i,1!"=="" set /a len+=1
set /a pad=box_width-2-len
if !pad! lss 0 set pad=0
set /a left=pad/2
set /a right=pad-left
:: If odd, right gets the extra space
if !pad! neq !left!+!right! set /a right=right+pad-left-right
set "spaces_left=!padstr:~0,%left%!"
set "spaces_right=!padstr:~0,%right%!"
set "line=║!spaces_left!!str!!spaces_right!║"
:: Always fill to box width
set "line=!line:~0,%box_width%!"
:: If too short, add extra space before last border
if not "!line:~77,1!"=="║" set "line=!line:~0,77!║"
echo    !line!
endlocal
goto :eof
:empty_center_box
endlocal
goto :eof

:skip_box
echo.

:: Quick loading sequence
echo    [INFO] Loading !TITLE! ...
echo.

:: Much faster progress bar
set "bar="
for /L %%j in (1,1,5) do (
    set /a "progress=%%j*20"
    set "bar=!bar!████"
    set "empty=    "
    set "remaining=!empty:~0,20!"
    set "remaining=!remaining:~%%j!"
    <nul set /p "=    Loading [!bar!!remaining!] !progress!%%!CR!"
    powershell -command "Start-Sleep -Milliseconds 1" >nul
)
echo.
echo    [✓] Loading completed!
echo.

:: Quick launch sequence
color 0A
echo    🚀 LAUNCHING !TITLE! ...
echo.

:: Simple spinner
set "spinner=/-\|"
for /L %%s in (1,1,8) do (
    set /a "spin=%%s%%4"
    for %%c in (!spin!) do (
        <nul set /p "=    !spinner:~%%c,1! Starting application...!CR!"
    )
    powershell -command "Start-Sleep -Milliseconds 10" >nul
)
echo.
echo    [SUCCESS] Ready to launch!
powershell -noprofile -command "[console]::Beep(800,80);[console]::Beep(1000,120)" >nul 2>&1
echo.

echo    [SYSTEM] Executing !PROGRAM_EXE! ...
echo.
title !TITLE! - Active Session
color 07
:: My.exe

echo.
color 0E
echo    [SYSTEM] Session completed.
echo    [INFO] End of luancher!
timeout /t 2 >nul
