
@echo off
:: Adaptive fullscreen/console size for any monitor
title lab12 - Geometry Tool
color 1E


:: Detect screen size and set mode con accordingly (use delayed expansion)
set "W="
set "H="
for /f "tokens=1,2 delims=x " %%a in ('powershell -noprofile -command "Add-Type -AssemblyName System.Windows.Forms; [System.Windows.Forms.Screen]::PrimaryScreen.Bounds.Size.ToString()"') do (
	set "W=%%a"
	set "H=%%b"
)
if not defined W set W=1366
if not defined H set H=768
setlocal ENABLEDELAYEDEXPANSION
set /a COLS=!W!/8
set /a LINES=!H!/16
if !COLS! lss 80 set COLS=80
if !LINES! lss 25 set LINES=25
mode con: cols=!COLS! lines=!LINES! >nul 2>&1

:: Try true fullscreen (legacy, works on some Windows, non-blocking)
start "" /min powershell -noprofile -command "$wshell = New-Object -ComObject wscript.shell; Start-Sleep -Milliseconds 200; $wshell.SendKeys('%{ENTER}')" >nul 2>&1

:: Fallback: maximize window if not true fullscreen
powershell -noprofile -command "$sig='using System;using System.Runtime.InteropServices;public class W{[DllImport(\"user32.dll\")]public static extern bool ShowWindow(IntPtr h,int n);[DllImport(\"user32.dll\")]public static extern IntPtr GetForegroundWindow();}';Add-Type $sig;[W]::ShowWindow([W]::GetForegroundWindow(),3)" >nul 2>&1

setlocal ENABLEDELAYEDEXPANSION
cls

:: Enable ANSI (for cursor hide) if supported
powershell -noprofile -command "$esc=[char]27;Write-Host -NoNewLine ($esc+'[?25l')" >nul 2>&1

:: Title typewriter effect
set "TITLE=Launching lab12 Geometry Tool"
for /l %%i in (1,1,40) do (
	set "chunk=!TITLE:~0,%%i!"
	<nul set /p =!chunk!                                        
	powershell -noprofile -command "[console]::Beep(600,18)" >nul 2>&1
	ping -n 1 127.0.0.1 >nul
)
echo(
echo   Author : oleksandkov
echo   Demo   : Spheres intersection / point containment
echo.

:: Single-line progress bar (0..100)
:: Dynamically sized progress bar (fills console width, no [filled...] artifacts)
for /f "tokens=2 delims=: " %%C in ('mode con ^| findstr /R /C:"Columns"') do set "WIDTH=%%C"
set /a PBARLEN=!WIDTH!-20
if !PBARLEN! lss 10 set PBARLEN=10
set "BAR="
for /l %%B in (1,1,!PBARLEN!) do set "BAR=!BAR!#"
for /l %%P in (0,2,100) do (
	set /a filled=!PBARLEN!*%%P/100
	set "seg=!BAR:~0,!filled!!"
	set /a spaces=!PBARLEN!-!filled!
	if !spaces! lss 0 set spaces=0
	set "pad="
	if !spaces! gtr 0 for /l %%S in (1,1,!spaces!) do set "pad=!pad! "
	<nul set /p =Loading [!seg!!pad!] %%P%%
	powershell -noprofile -command "[console]::Beep(400+%%P*3,10)" >nul 2>&1
	ping -n 1 127.0.0.1 >nul
)
:: After loop, print a clean line to ensure no \r remains
echo(
echo
echo Launching...
powershell -noprofile -command "[console]::Beep(1000,160);[console]::Beep(1400,180)" >nul 2>&1
timeout /t 1 >nul

:: Run the program
"%~dp0lab12.exe"

:: Restore cursor
powershell -noprofile -command "$esc=[char]27;Write-Host ($esc+'[?25h')" >nul 2>&1
echo.
echo Session finished. Press any key to exit.
pause >nul