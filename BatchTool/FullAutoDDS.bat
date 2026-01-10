@echo off
setlocal ENABLEDELAYEDEXPANSION

set "TEXCONV=C:\Tools\texconv.exe"

set /p TARGET_SIZE=Target resize size for Normal/ORM (128, 256, 512, 1024): 
set /p TARGET_DIR=Texture folder path: 

echo.
echo ===== START =====
echo Folder : %TARGET_DIR%
echo Resize : %TARGET_SIZE%x%TARGET_SIZE% (Normal / ORM)
echo.
echo Normal  (*_TN / *_N)   -> DDS BC5  + resize + MIP=1
echo ORM     (*_TAoRM)      -> DDS BC3  + resize + MIP=1
echo Diffuse (*_TD)         -> DDS BC7(sRGB) / keep max size / MIP=1
echo.

:: ===== NORMAL : *_TN.png =====
for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_TN.png" 2^>nul
') do (
    echo [NORMAL RESIZE / BC5 / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC5_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== NORMAL : *_N.png  (exclude *_TN to avoid duplicate) =====
for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_N.png" 2^>nul
') do (
    echo %%F | find "_TN" >nul
    if errorlevel 1 (
        echo [NORMAL RESIZE / BC5 / MIP1] %%F
        "%TEXCONV%" ^
            -w %TARGET_SIZE% -h %TARGET_SIZE% ^
            -m 1 ^
            -f BC5_UNORM ^
            -ft dds ^
            -y ^
            "%TARGET_DIR%\%%F"
    )
)

:: ===== ORM : *_TAoRM.png =====
for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_TAoRM.png" 2^>nul
') do (
    echo [ORM RESIZE / BC3 / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC3_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_ORM.png" 2^>nul
') do (
    echo [ORM RESIZE / BC3 / MIP1] %%F
    "%TEXCONV%" ^
        -w %TARGET_SIZE% -h %TARGET_SIZE% ^
        -m 1 ^
        -f BC3_UNORM ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

:: ===== DIFFUSE : *_TD.png (KEEP MAX SIZE + MIP 1) =====
for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_TD.png" 2^>nul
') do (
    echo [DIFFUSE KEEP / BC7_SRGB / MIP1] %%F
    "%TEXCONV%" ^
        -m 1 ^
        -f BC7_UNORM_SRGB ^
        -ft dds ^
        -y ^
        "%TARGET_DIR%\%%F"
)

for /f "delims=" %%F in ('
    dir /b /a-d "%TARGET_DIR%\*_A.png" 2^>nul
') do (
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
