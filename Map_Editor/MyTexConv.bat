@echo off


set TEXCONV= "C:\Users\gimch\OneDrive\문서\GitHub\154Final_GameUnknown\Map_Editor"
set OUTDIR=  ".\out"

if not exist %OUTDIR% mkdir %OUTDIR%

for %%f in (%*) do (
    echo 변환중: %%~nxf
    %TEXCONV% -w 512 -h 512 -m 1 -f BC7_UNORM %%f -o %OUTDIR% -y
)

echo 변환 완료

pause