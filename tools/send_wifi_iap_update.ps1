param(
    [string]$HostName = "192.168.4.1",
    [int]$Port = 8080,
    [string]$Package = "D:\STMPRO\ai_project\MDK-ARM\app_ota\app.bin",
    [int]$ChunkSize = 1024
)

if (-not (Test-Path -LiteralPath $Package)) {
    throw "Package not found: $Package"
}

$bytes = [System.IO.File]::ReadAllBytes($Package)
if ($bytes.Length -lt 16) {
    throw "Package too small: $Package"
}

$client = New-Object System.Net.Sockets.TcpClient

try {
    Write-Host "[PC] Connect your computer to WiFi SSID: STM32-IAP, password: 12345678"
    Write-Host "[PC] Connecting to $HostName`:$Port ..."
    $client.Connect($HostName, $Port)
    $stream = $client.GetStream()
    $stream.WriteTimeout = 5000
    $stream.ReadTimeout = 30000

    function Wait-TcpToken {
        param([string]$Token)

        $buf = New-Object byte[] 128
        $text = ""
        while (-not $text.Contains($Token)) {
            $n = $stream.Read($buf, 0, $buf.Length)
            if ($n -le 0) {
                throw "TCP connection closed while waiting for $Token"
            }
            $chunk = [System.Text.Encoding]::ASCII.GetString($buf, 0, $n)
            Write-Host -NoNewline $chunk
            $text += $chunk
            if ($text.Length -gt 1024) {
                $text = $text.Substring($text.Length - 512)
            }
        }
    }

    Write-Host "[PC] Connected, sending IAP command..."
    $iap = [System.Text.Encoding]::ASCII.GetBytes("IAP")
    $stream.Write($iap, 0, $iap.Length)
    Wait-TcpToken -Token "READY"

    Write-Host "[PC] Sending package: $Package"
    Write-Host "[PC] Bytes: $($bytes.Length)"
    Write-Host "[PC] Chunk: $ChunkSize bytes, bootloader-paced"

    $stream.Write($bytes, 0, 16)

    $offset = 16
    while ($offset -lt $bytes.Length) {
        Wait-TcpToken -Token "NEXT"

        $count = [Math]::Min($ChunkSize, $bytes.Length - $offset)
        $stream.Write($bytes, $offset, $count)
        $offset += $count

        if (($offset % 16384) -eq 0 -or $offset -eq $bytes.Length) {
            Write-Host "[PC] Sent $offset / $($bytes.Length)"
        }
    }

    Write-Host "[PC] Send done. Check COM9 bootloader log for [WIFI] update done."
}
finally {
    if ($stream) {
        $stream.Dispose()
    }
    $client.Close()
}
