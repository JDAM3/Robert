@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by RAILDIAG.HPJ. >"hlp\RailDiag.hm"
echo. >>"hlp\RailDiag.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\RailDiag.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\RailDiag.hm"
echo. >>"hlp\RailDiag.hm"
echo // Prompts (IDP_*) >>"hlp\RailDiag.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\RailDiag.hm"
echo. >>"hlp\RailDiag.hm"
echo // Resources (IDR_*) >>"hlp\RailDiag.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\RailDiag.hm"
echo. >>"hlp\RailDiag.hm"
echo // Dialogs (IDD_*) >>"hlp\RailDiag.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\RailDiag.hm"
echo. >>"hlp\RailDiag.hm"
echo // Frame Controls (IDW_*) >>"hlp\RailDiag.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\RailDiag.hm"
REM -- Make help for Project RAILDIAG


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\RailDiag.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\RailDiag.hlp" goto :Error
if not exist "hlp\RailDiag.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\RailDiag.hlp" Debug
if exist Debug\nul copy "hlp\RailDiag.cnt" Debug
if exist Release\nul copy "hlp\RailDiag.hlp" Release
if exist Release\nul copy "hlp\RailDiag.cnt" Release
echo.
goto :done

:Error
echo hlp\RailDiag.hpj(1) : error: Problem encountered creating help file

:done
echo.
