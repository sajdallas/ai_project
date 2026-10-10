$project = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$charsPath = Join-Path $PSScriptRoot "ui_cjk_chars.txt"
$outPath = Join-Path $project "LVGL\src\font\lv_font_cjk16.c"

if (-not (Test-Path -LiteralPath $charsPath)) {
    throw "Character list not found: $charsPath"
}

$chars = Get-Content -LiteralPath $charsPath -Raw -Encoding UTF8

npx lv_font_conv `
    --font C:/Windows/Fonts/simhei.ttf `
    --size 16 `
    --bpp 1 `
    --range 0x20-0x7F,0x2000-0x206F,0x3000-0x303F,0xFF00-0xFFEF `
    --symbols $chars `
    --format lvgl `
    --lv-font-name lv_font_cjk16 `
    -o $outPath

if ($LASTEXITCODE -ne 0) {
    throw "lv_font_conv failed with exit code $LASTEXITCODE"
}

Write-Host "Generated $outPath"
