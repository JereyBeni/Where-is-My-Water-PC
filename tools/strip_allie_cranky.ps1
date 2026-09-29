# Strip Allie & Cranky content from an assets/ tree (Swampy-only).
# Does NOT delete shared Objects that Swampy levels still reference
# (e.g. broken_pipe_cranky.hs).
#
# Usage:
#   .\tools\strip_allie_cranky.ps1 -AssetsRoot .\assets
#   .\tools\strip_allie_cranky.ps1 -AssetsRoot .\app\src\main\assets -WhatIf

param(
    [Parameter(Mandatory = $true)]
    [string] $AssetsRoot,
    [switch] $WhatIf
)

if (-not (Test-Path $AssetsRoot)) {
    Write-Error "Assets root not found: $AssetsRoot"
    exit 1
}

$removed = 0
$bytes = [int64]0

function Remove-Match {
    param([string]$Path)
    if (-not (Test-Path $Path)) { return }
    $item = Get-Item $Path
    if ($item.PSIsContainer) {
        $size = (Get-ChildItem $Path -Recurse -File -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
        if (-not $size) { $size = 0 }
        Write-Host "[DIR ] $Path"
        if (-not $WhatIf) { Remove-Item -Recurse -Force $Path }
        $script:removed++
        $script:bytes += $size
    } else {
        Write-Host "[FILE] $Path"
        if (-not $WhatIf) { Remove-Item -Force $Path }
        $script:removed++
        $script:bytes += $item.Length
    }
}

# --- Character-exclusive folders ---
Remove-Match (Join-Path $AssetsRoot "Audio\Allie")
Remove-Match (Join-Path $AssetsRoot "Audio\Cranky")

# --- Animations ---
Get-ChildItem (Join-Path $AssetsRoot "Animations") -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match '^(allie|cranky)' } |
    ForEach-Object { Remove-Match $_.FullName }

# --- Levels (name-based packs) ---
Get-ChildItem (Join-Path $AssetsRoot "Levels") -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match '^(allie|cranky|LOW_Allie|LOW_Cranky)' } |
    ForEach-Object { Remove-Match $_.FullName }

# --- Data UI / IAP / world select for DLC characters ---
Get-ChildItem (Join-Path $AssetsRoot "Data") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Allie|Cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

Get-ChildItem (Join-Path $AssetsRoot "Data\Notifications") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Allie|Cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

# --- Music / upsell textures named after characters ---
Get-ChildItem (Join-Path $AssetsRoot "Audio\Music") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match '^(Allie_|Cranky_)' } |
    ForEach-Object { Remove-Match $_.FullName }

Get-ChildItem (Join-Path $AssetsRoot "Textures") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'allie|cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

# --- Character-specific duck / win stingers (safe name filter) ---
Get-ChildItem (Join-Path $AssetsRoot "Audio\Duckies") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Allie|Cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

Get-ChildItem (Join-Path $AssetsRoot "Audio\GameWin") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Allie|Cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

Get-ChildItem (Join-Path $AssetsRoot "Audio\Liquids") -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match 'Allie|Cranky' } |
    ForEach-Object { Remove-Match $_.FullName }

# NOTE: assets/Objects/*cranky* are KEPT — Swampy levels reference some of them.

$mb = [math]::Round($bytes / 1MB, 2)
Write-Host ""
Write-Host "Done. Removed entries: $removed  (~$mb MB)"
if ($WhatIf) { Write-Host "(WhatIf — nothing deleted)" }
