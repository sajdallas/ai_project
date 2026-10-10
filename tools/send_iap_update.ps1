param(
    [string]$PortName = "COM9",
    [int]$BaudRate = 115200,
    [string]$Package = "D:\STMPRO\ai_project\MDK-ARM\app_ota\app.bin",
    [int]$HandshakeSeconds = 15
)

if (-not (Test-Path -LiteralPath $Package)) {
    throw "Package not found: $Package"
}

$bytes = [System.IO.File]::ReadAllBytes($Package)
if ($bytes.Length -lt 16) {
    throw "Package too small: $Package"
}

$port = New-Object System.IO.Ports.SerialPort $PortName,$BaudRate,'None',8,'One'
$port.ReadTimeout = 100
$port.WriteTimeout = 3000
$port.Open()

try {
    function Wait-Token {
        param(
            [string]$Token,
            [int]$TimeoutSeconds = 20
        )

        $text = ""
        $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
        while ((Get-Date) -lt $deadline) {
            $chunk = $port.ReadExisting()
            if ($chunk.Length -gt 0) {
                Write-Host -NoNewline $chunk
                $text += $chunk
                if ($text.Contains($Token)) {
                    return $true
                }
                if ($text.Length -gt 4096) {
                    $text = $text.Substring($text.Length - 2048)
                }
            }
            Start-Sleep -Milliseconds 20
        }

        return $false
    }

    Write-Host "[PC] Opened $PortName @$BaudRate"
    Write-Host "[PC] Reset the board now, waiting for [IAP] ready..."

    $ready = $false
    $rx = ""
    $deadline = (Get-Date).AddSeconds($HandshakeSeconds)

    while ((Get-Date) -lt $deadline) {
        $port.Write("IAP")
        Start-Sleep -Milliseconds 200

        $chunk = $port.ReadExisting()
        if ($chunk.Length -gt 0) {
            Write-Host -NoNewline $chunk
            $rx += $chunk
            if ($rx.Contains("[IAP] ready")) {
                $ready = $true
                break
            }
            if ($rx.Length -gt 4096) {
                $rx = $rx.Substring($rx.Length - 2048)
            }
        }
    }

    if (-not $ready) {
        throw "IAP handshake timeout"
    }

    Write-Host ""
    Write-Host "[PC] Sending package: $Package"
    Write-Host "[PC] Bytes: $($bytes.Length)"

    $port.Write($bytes, 0, 16)

    $offset = 16
    $chunkSize = 1024
    while ($offset -lt $bytes.Length) {
        if (-not (Wait-Token -Token "[IAP] next" -TimeoutSeconds 30)) {
            throw "Timed out waiting for bootloader chunk request"
        }

        $count = [Math]::Min($chunkSize, $bytes.Length - $offset)
        $port.Write($bytes, $offset, $count)
        $offset += $count

        if (($offset % 16384) -eq 0 -or $offset -eq $bytes.Length) {
            Write-Host "[PC] Sent $offset / $($bytes.Length)"
        }
    }

    Write-Host "[PC] Send done, reading bootloader log..."
    $deadline = (Get-Date).AddSeconds(30)
    while ((Get-Date) -lt $deadline) {
        try {
            if (-not $port.IsOpen) {
                break
            }

            $chunk = $port.ReadExisting()
            if ($chunk.Length -gt 0) {
                Write-Host -NoNewline $chunk
            }
        }
        catch [System.InvalidOperationException] {
            Write-Host ""
            Write-Host "[PC] Serial port closed, log reading stopped."
            break
        }
        Start-Sleep -Milliseconds 100
    }
}
finally {
    if ($port.IsOpen) {
        $port.Close()
    }
}
