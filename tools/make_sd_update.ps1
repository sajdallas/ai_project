param(
    [Parameter(Mandatory = $true)]
    [string]$InputBin,

    [string]$Output = "app.bin"
)

$ErrorActionPreference = "Stop"

function Update-Crc32 {
    param(
        [UInt32]$Crc,
        [byte[]]$Bytes,
        [int]$Count
    )

    $mask = [UInt64]4294967295
    $poly = [UInt64]3988292384
    $crcLocal = ([UInt64]$Crc) -bxor $mask
    for ($i = 0; $i -lt $Count; $i++) {
        $crcLocal = ($crcLocal -bxor [UInt64]$Bytes[$i]) -band $mask
        for ($bit = 0; $bit -lt 8; $bit++) {
            if (($crcLocal -band 1) -ne 0) {
                $crcLocal = (($crcLocal -shr 1) -bxor $poly) -band $mask
            } else {
                $crcLocal = ($crcLocal -shr 1) -band $mask
            }
        }
    }

    return [UInt32](($crcLocal -bxor $mask) -band $mask)
}

$inputPath = (Resolve-Path $InputBin).Path
$payload = [System.IO.File]::ReadAllBytes($inputPath)
if ($payload.Length -eq 0) {
    throw "Input binary is empty."
}

$crc = Update-Crc32 -Crc 0 -Bytes $payload -Count $payload.Length

$outStream = [System.IO.File]::Open($Output, [System.IO.FileMode]::Create)
try {
    $writer = New-Object System.IO.BinaryWriter($outStream)
    $writer.Write([UInt32]0x41544F53)
    $writer.Write([UInt32]1)
    $writer.Write([UInt32]$payload.Length)
    $writer.Write([UInt32]$crc)
    $writer.Write($payload)
    $writer.Flush()
} finally {
    $outStream.Close()
}

Write-Host ("Payload : {0}" -f $inputPath)
Write-Host ("Size    : {0} bytes" -f $payload.Length)
Write-Host ("CRC32   : 0x{0:X8}" -f $crc)
Write-Host ("Package : {0}" -f (Resolve-Path $Output).Path)
