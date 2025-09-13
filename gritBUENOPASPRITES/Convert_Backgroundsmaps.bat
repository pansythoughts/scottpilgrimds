@echo off
cls
del backgrounds\*.img
del backgrounds\*.map
del backgrounds\*.pal
del h\*.h
grit.exe bmp\*.png -g -gT00F800 -m -mRtf -mLs -p -ftb

echo grit.exe bmp\*.png -g -gT00F800 -p -ftb
for /f "delims==" %%F in ('dir /b *.bin') do ren "%%~nxF" "%%~nF"
move *.img backgrounds
move *.map backgrounds
move *.pal backgrounds
move *.h h
pause
exit