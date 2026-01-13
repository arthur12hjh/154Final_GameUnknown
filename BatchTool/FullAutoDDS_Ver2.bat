@echo off
setlocal EnableDelayedExpansion

set "TEXCONV=C:\Tools\texconv.exe"

if not exist "%TEXCONV%" (
    echo [ERROR] texconv not found: "%TEXCONV%"
    pause
    exit /b 1
)

set /p TARGET_SIZE=Target resize size for Normal/ORM (512, 1024, 2048): 
set /p TARGET_DIR=Texture folder path: 

echo.
echo ===== START =====
echo Folder : %TARGET_DIR%
echo Normal/ORM Resize : %TARGET_SIZE%x%TARGET_SIZE%
echo Diffuse : KEEP ORIGINAL SIZE
echo.
echo Normal  (*_TN / *_N)     -> DDS BC7_UNORM      + resize + MIP=1
echo ORM     (*_TAoRM, *_ORM) -> DDS BC3_UNORM      + resize + MIP=1
echo Diffuse (*_TD, *_A)      -> DDS BC7_UNORM_SRGB + KEEP   + MIP=1
echo.

:: ===== NORMAL : *_TN.png =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_TN.png" 2^>nul') do (
    echo [NORMAL RESIZE / BC7_UNORM / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC7_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== NORMAL : *_N.png  (exclude *_TN to avoid duplicate) =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_N.png" 2^>nul') do (
    echo %%F | find "_TN" >nul
    if errorlevel 1 (
        echo [NORMAL RESIZE / BC7_UNORM / MIP1] %%F
        "%TEXCONV%" ^
            -w %TARGET_SIZE% -h %TARGET_SIZE% ^
            -m 1 ^
            -f BC7_UNORM ^
            -ft dds ^
            -y ^
            "%TARGET_DIR%\%%F"
    )
)

:: ===== ORM : *_TAoRM.png =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_TAoRM.png" 2^>nul') do (
    echo [ORM RESIZE / BC3_UNORM / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC3_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== ORM : *_ORM.png =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_ORM.png" 2^>nul') do (
    echo [ORM RESIZE / BC3_UNORM / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC3_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== DIFFUSE : *_TD.png (KEEP ORIGINAL SIZE) =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_TD.png" 2^>nul') do (
    echo [DIFFUSE KEEP / BC7_SRGB / MIP1] %%F
    "%TEXCONV%" ^
        -m 1 ^
        -f BC7_UNORM_SRGB ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== DIFFUSE : *_A.png (KEEP ORIGINAL SIZE) =====
for /f "delims=" %%F in ('dir /b /a-d "%TARGET_DIR%\*_A.png" 2^>nul') do (
    echo [DIFFUSE KEEP / BC7_SRGB / MIP1] %%F
    "%TEXCONV%" ^
        -m 1 ^
        -f BC7_UNORM_SRGB ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

echo.
echo DONE
pause
endlocal
