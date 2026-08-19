@echo off
cd /d d:\STMPRO\ai_project
npx lv_font_conv --font C:/Windows/Fonts/simhei.ttf --size 16 --bpp 1 --range 0x20-0x7F,0x2000-0x206F,0x3000-0x303F,0xFF00-0xFFEF --format lvgl --lv-font-name lv_font_punct16 -o LVGL\src\font\lv_font_punct16.c
echo DONE
pause