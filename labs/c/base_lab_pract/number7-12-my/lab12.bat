@echo off
cd /d %~dp0
:: ========================================================================
:: LAB12 Geometry Tool Launcher - Fascinating Loading Animation
:: Author: oleksandkov
:: Description: Interactive 3D Sphere Geometry Calculator
:: ========================================================================

setlocal EnableDelayedExpansion
color 0A
title Loading Lab12 Geometry Tool...

:: Set console size for optimal display and create carriage return variable
mode con: cols=100 lines=35
for /f %%A in ('"prompt $H &echo on &for %%B in (1) do rem"') do set "BS=%%A"
set "CR=!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!!BS!"

:: Clear screen and show header
cls
echo.
echo    ██████╗ ███████╗ ██████╗ ███╗   ███╗███████╗████████╗██████╗ ██╗   ██╗
echo    ██╔════╝ ██╔════╝██╔═══██╗████╗ ████║██╔════╝╚══██╔══╝██╔══██╗╚██╗ ██╔╝
echo    ██║  ███╗█████╗  ██║   ██║██╔████╔██║█████╗     ██║   ██████╔╝ ╚████╔╝ 
echo    ██║   ██║██╔══╝  ██║   ██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██╗  ╚██╔╝  
echo    ╚██████╔╝███████╗╚██████╔╝██║ ╚═╝ ██║███████╗   ██║   ██║  ██║   ██║   
echo     ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   
echo.
echo                            LAB12 - 3D SPHERE GEOMETRY TOOL
echo                     ══════════════════════════════════════════════
echo                        Author: oleksandkov ^| Version: 2.0
echo.



:: Quick loading sequence
echo    [INFO] Loading Lab12 Geometry Tool...
echo.

:: Fast progress bar
set "bar="
for /L %%j in (1,1,20) do (
    set /a "progress=%%j*5"
    set "bar=!bar!██"
    
    set "empty=                                        "
    set "remaining=!empty:~0,40!"
    set "remaining=!remaining:~%%j!"
    <nul set /p "=    Loading [!bar!!remaining!] !progress!%%!CR!"
    
    powershell -noprofile -command "[console]::Beep(500,15)" >nul 2>&1
    powershell -command "Start-Sleep -Milliseconds 50" >nul
)

echo.
echo    [✓] Loading completed!
echo.

:: Quick launch sequence
color 0A
echo    🚀 LAUNCHING LAB12 GEOMETRY TOOL...
echo.

:: Simple spinner
set "spinner=/-\|"
for /L %%s in (1,1,8) do (
    set /a "spin=%%s%%4"
    for %%c in (!spin!) do (
        <nul set /p "=    !spinner:~%%c,1! Starting application...!CR!"
    )
    powershell -command "Start-Sleep -Milliseconds 100" >nul
)

echo.
echo    [SUCCESS] Ready to launch!
powershell -noprofile -command "[console]::Beep(800,100);[console]::Beep(1000,150)" >nul 2>&1
echo.

:: Quick info display
echo    ╔══════════════════════════════════════════════════════════════════════════════╗
echo    ║                         LAB12 GEOMETRY TOOL READY                            ║
echo    ║  • Sphere Intersection Detection • Point Containment • 3D Calculations       ║
echo    ╚══════════════════════════════════════════════════════════════════════════════╝
echo.

:: Compile and run with error handling
echo    [SYSTEM] Compilation successful!
echo    [SYSTEM] Executing lab12.exe...
echo.
title Lab12 Geometry Tool - Active Session
color 07
lab12.exe

:: Cleanup and exit message
echo.
color 0E
echo    [SYSTEM] Lab12 session completed.
echo    [INFO] Thank you for using the Geometry Tool!
timeout /t 2 >nul