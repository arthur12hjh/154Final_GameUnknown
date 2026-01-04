@echo off
chcp 65001 >nul
setlocal ENABLEDELAYEDEXPANSION

set "TEXCONV=C:\Tools\texconv.exe"

IF NOT EXIST "%TEXCONV%" (
    echo texconv.exe 경로가 잘못되었습니다.
    echo 현재 설정: %TEXCONV%
    pause
    exit /b
)

echo.
set /p TARGET_W=가로 해상도 입력 (예: 1024): 
set /p TARGET_H=세로 해상도 입력 (예: 1024): 

echo.
set /p INPUT_DIR=리사이즈할 PNG 폴더 경로: 
set /p OUTPUT_DIR=출력 폴더 경로: 

echo.
echo ===== PNG Resize 실행 =====
echo 입력 폴더 : %INPUT_DIR%
echo 출력 폴더 : %OUTPUT_DIR%
echo 해상도    : %TARGET_W%x%TARGET_H%
echo =================================
echo.

"%TEXCONV%" ^
 -ft png ^
 -w %TARGET_W% ^
 -h %TARGET_H% ^
 -if FANT ^
 -y ^
 -o "%OUTPUT_DIR%" ^
 "%INPUT_DIR%\*.png"

echo.
echo 완료되었습니다.
pause
endlocal
