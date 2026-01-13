@echo off
setlocal EnableDelayedExpansion

REM =========================
REM DDS Batch Resizer (texconv)
REM =========================

set "TEXCONV=C:\Tools\texconv.exe"

if not exist "%TEXCONV%" (
    echo [ERROR] texconv not found: "%TEXCONV%"
    pause
    exit /b 1
)

echo.
set /p "TARGET_SIZE=Target size (e.g. 256 / 512 / 1024 / 2048): "
if "%TARGET_SIZE%"=="" (
    echo [ERROR] Target size is empty.
    pause
    exit /b 1
)

set /p "TARGET_DIR=Folder path that contains DDS files: "
if "%TARGET_DIR%"=="" (
    echo [ERROR] Folder path is empty.
    pause
    exit /b 1
)

if not exist "%TARGET_DIR%" (
    echo [ERROR] Folder not found: "%TARGET_DIR%"
    pause
    exit /b 1
)

set "OUTDIR=%TARGET_DIR%\_resized_DDS_%TARGET_SIZE%"
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

echo.
echo ===== START =====
echo Input  : "%TARGET_DIR%"
echo Output : "%OUTDIR%"
echo Resize : %TARGET_SIZE%x%TARGET_SIZE%
echo Target : *.dds
echo.

REM ---- Optional: force format (uncomment ONE if you want) ----
REM set "FORCE_FMT=-f BC7_UNORM"
REM set "FORCE_FMT=-f BC7_UNORM_SRGB"
REM set "FORCE_FMT=-f BC5_UNORM"
REM set "FORCE_FMT=-f BC3_UNORM"
set "FORCE_FMT="

REM ---- Mip control ----
REM -m 1 : only top mip
REM if you want full mip chain, change to: set "MIP_OPT=" (empty) or -m 0
set "MIP_OPT=-m 1"

REM ---- Iterate all DDS ----
set /a COUNT=0
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*.dds" 2^>nul') do (
    set /a COUNT+=1
    echo [!COUNT!] Resizing: %%F

    "%TEXCONV%" ^
        -nologo ^
        -y ^
        -o "%OUTDIR%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        %MIP_OPT% ^
        %FORCE_FMT% ^
        "%TARGET_DIR%\%%F"
)

if %COUNT%==0 (
    echo.
    echo [WARN] No .dds files found in: "%TARGET_DIR%"
) else (
    echo.
    echo ===== DONE =====
    echo Resized %COUNT% file(s).
)

pause
endlocal
