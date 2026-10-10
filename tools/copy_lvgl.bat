@echo off
set SRC=d:\STMPRO\ai_project\LVGL\src
set DST=d:\STMPRO\ai_project\LVGL\lvgl_src

if not exist "%DST%" mkdir "%DST%"

echo Copying LVGL source files...

copy "%SRC%\*.c" "%DST%\" >nul
copy "%SRC%\*.h" "%DST%\" >nul

copy "%SRC%\core\*.c" "%DST%\" >nul
copy "%SRC%\core\*.h" "%DST%\" >nul

copy "%SRC%\draw\*.c" "%DST%\" >nul
copy "%SRC%\draw\*.h" "%DST%\" >nul
copy "%SRC%\draw\sw\*.c" "%DST%\" >nul
copy "%SRC%\draw\sw\*.h" "%DST%\" >nul

copy "%SRC%\font\*.c" "%DST%\" >nul
copy "%SRC%\font\*.h" "%DST%\" >nul

copy "%SRC%\hal\*.c" "%DST%\" >nul
copy "%SRC%\hal\*.h" "%DST%\" >nul

copy "%SRC%\misc\*.c" "%DST%\" >nul
copy "%SRC%\misc\*.h" "%DST%\" >nul

copy "%SRC%\extra\*.c" "%DST%\" >nul
copy "%SRC%\extra\*.h" "%DST%\" >nul
copy "%SRC%\extra\themes\default\*.c" "%DST%\" >nul
copy "%SRC%\extra\themes\default\*.h" "%DST%\" >nul
copy "%SRC%\extra\themes\basic\*.c" "%DST%\" >nul
copy "%SRC%\extra\themes\basic\*.h" "%DST%\" >nul

copy "d:\STMPRO\ai_project\HARDWARE\LVGL\*.c" "%DST%\" >nul

echo Done! All files copied to: %DST%
echo Now in Keil, add all .c files from this folder to your project.
pause