# Optimize shared assets for the PC port package (size only — no format invention).
#
# Safe reductions based on repo inventory:
#   - Textures/*-HD.* when a non-HD pair exists (~23 MB)
#   - Localized texture suffixes (-ru, -zh-hans, -de, ...)
#   - Mystery Duck character packs (out of Swampy-only scope)
#   - Editor leftovers: *.psd, *.bak
#
# Usage:
#   .\tools\optimize_shared_assets.ps1 -AssetsRoot .\assets
#   .\tools\optimize_shared_assets.ps1 -AssetsRoot .\assets -WhatIf

param(
    [Parameter(Mandatory = $true)]
    [string] $AssetsRoot,
    [switch] $WhatIf
)

if (-not (Test-Path $AssetsRoot)) {
    Write-Error "Assets root not found: $AssetsRoot"
    exit 1
}

$script:removed = 0
$script:bytes = [int64]0

function Remove-PathEntry {
    param([string]$Path)
    if (-not (Test-Path $Path)) { return }
    $item = Get-Item $Path -Force
    if ($item.PSIsContainer) {
        $size = (Get-ChildItem $Path -Recurse -File -Force -ErrorAction SilentlyContinue |
            Measure-Object -Property Length -Sum).Sum
        if (-not $size) { $size = 0 }
        Write-Host "[DIR ] $Path"
        if (-not $WhatIf) { Remove-Item -Recurse -Force $Path }
        $script:removed++
        $script:bytes += [int64]$size
    } else {
        Write-Host "[FILE] $Path"
        if (-not $WhatIf) { Remove-Item -Force $Path }
        $script:removed++
        $script:bytes += [int64]$item.Length
    }
}

# ---------------------------------------------------------------------------
# 1) Paired HD textures — only delete -HD when non-HD sibling exists
# ---------------------------------------------------------------------------
$texDir = Join-Path $AssetsRoot "Textures"
if (Test-Path $texDir) {
    $byName = @{}
    Get-ChildItem $texDir -File -ErrorAction SilentlyContinue | ForEach-Object {
        $byName[$_.Name] = $_
    }
    foreach ($name in @($byName.Keys)) {
        if ($name -notmatch '-HD\.') { continue }
        $baseName = $name -replace '-HD\.', '.'
        if ($byName.ContainsKey($baseName)) {
            Remove-PathEntry $byName[$name].FullName
        }
    }
}

# ---------------------------------------------------------------------------
# 2) Localized texture variants (keep un-suffixed / default)
#    Patterns observed in repo: -ru, -zh-hans, -zh-hant, -de, -fr, -es, -it,
#    -ja, -ko, -pt, -nl, -sv, -pl, -tr, ... and -XX-HD already handled above
# ---------------------------------------------------------------------------
if (Test-Path $texDir) {
    $locale = [regex]'(?i)-(ru|de|fr|es|it|ja|ko|pt|nl|sv|pl|tr|ar|th|id|ms|hi|zh-hans|zh-hant|zh)(?=-HD\.|\.|$)'
    Get-ChildItem $texDir -File -ErrorAction SilentlyContinue |
        Where-Object { $locale.IsMatch($_.Name) } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

# ---------------------------------------------------------------------------
# 3) Mystery Duck (DLC character) — same policy as Allie/Cranky
# ---------------------------------------------------------------------------
Remove-PathEntry (Join-Path $AssetsRoot "Audio\Mystery")

$animDir = Join-Path $AssetsRoot "Animations"
if (Test-Path $animDir) {
    Get-ChildItem $animDir -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)^mystery' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

$levelsDir = Join-Path $AssetsRoot "Levels"
if (Test-Path $levelsDir) {
    Get-ChildItem $levelsDir -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)^(mystery|LOW_Mystery)' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

$dataDir = Join-Path $AssetsRoot "Data"
if (Test-Path $dataDir) {
    Get-ChildItem $dataDir -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)Mystery' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
    $notif = Join-Path $dataDir "Notifications"
    if (Test-Path $notif) {
        Get-ChildItem $notif -File -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '(?i)Mystery' } |
            ForEach-Object { Remove-PathEntry $_.FullName }
    }
}

$musicDir = Join-Path $AssetsRoot "Audio\Music"
if (Test-Path $musicDir) {
    Get-ChildItem $musicDir -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)mystery' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

$curvesDir = Join-Path $AssetsRoot "Curves"
if (Test-Path $curvesDir) {
    Get-ChildItem $curvesDir -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)mystery' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

if (Test-Path $texDir) {
    Get-ChildItem $texDir -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '(?i)mystery' } |
        ForEach-Object { Remove-PathEntry $_.FullName }
}

# ---------------------------------------------------------------------------
# 4) Runtime-irrelevant editor leftovers
# ---------------------------------------------------------------------------
Get-ChildItem $AssetsRoot -Recurse -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Extension -match '(?i)\.(psd|bak)$' } |
    ForEach-Object { Remove-PathEntry $_.FullName }

# ---------------------------------------------------------------------------
# 5) Non-game packaging leftovers inside assets
# ---------------------------------------------------------------------------
Remove-PathEntry (Join-Path $AssetsRoot "offline.html")
Remove-PathEntry (Join-Path $AssetsRoot "wmw-extra.zip")

$mb = [math]::Round($script:bytes / 1MB, 2)
Write-Host ""
Write-Host "Shared optimize done. Removed entries: $($script:removed)  (~$mb MB)"
if ($WhatIf) { Write-Host "(WhatIf — nothing deleted)" }
