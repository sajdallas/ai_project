@echo off
cd /d d:\STMPRO\ai_project

:: ============================================================
:: 标点字库（小文件，秒级生成）
:: ============================================================
npx lv_font_conv --font C:/Windows/Fonts/simhei.ttf --size 16 --bpp 1 --range 0x20-0x7F,0x2000-0x206F,0x3000-0x303F,0xFF00-0xFFEF --symbols ℃° --format lvgl --lv-font-name lv_font_punct16 -o LVGL\src\font\lv_font_punct16.c
echo PUNCT16 DONE

:: ============================================================
:: CJK 中文字库（大文件，耗时较长，需要时取消注释）
:: 用 PowerShell 绕过 CMD 长度限制，从 gb2312_l1.txt 读取字符
:: ============================================================
powershell -ExecutionPolicy Bypass -Command "$chars = Get-Content gb2312_l1.txt -Raw -Encoding UTF8; npx lv_font_conv --font C:/Windows/Fonts/simhei.ttf --size 16 --bpp 1 --range 0x20-0x7F,0x2000-0x206F,0x3000-0x303F,0xFF00-0xFFEF --symbols $chars --format lvgl --lv-font-name lv_font_cjk16 -o LVGL\src\font\lv_font_cjk16.c"

echo ALL DONE
pause