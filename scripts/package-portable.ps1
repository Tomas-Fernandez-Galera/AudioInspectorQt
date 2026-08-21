param(
    [string]$QtRoot = "C:\Qt\6.10.2\mingw_64"
)

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDirectory = Join-Path $projectRoot "build-package"
$distRoot = Join-Path $projectRoot "dist"
$packageName = "AudioInspectorQt-0.1.0-Windows-x64"
$packageDirectory = Join-Path $distRoot $packageName
$zipPath = Join-Path $distRoot ($packageName + ".zip")

$qmake = Join-Path $QtRoot "bin\qmake.exe"
$deploy = Join-Path $QtRoot "bin\windeployqt.exe"
$make = "C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe"

foreach ($tool in @($qmake, $deploy, $make)) {
    if (-not (Test-Path -LiteralPath $tool -PathType Leaf)) {
        throw "Required tool not found: $tool"
    }
}

# Both targets are fixed children of the project directory. Resolving and
# checking them first prevents an accidental broad deletion if paths change.
$resolvedProject = [System.IO.Path]::GetFullPath($projectRoot)
$resolvedBuild = [System.IO.Path]::GetFullPath($buildDirectory)
$resolvedDist = [System.IO.Path]::GetFullPath($distRoot)
foreach ($target in @($resolvedBuild, $resolvedDist)) {
    if (-not $target.StartsWith($resolvedProject + [System.IO.Path]::DirectorySeparatorChar,
                               [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Unsafe output path: $target"
    }
}

if (Test-Path -LiteralPath $buildDirectory) { Remove-Item -LiteralPath $buildDirectory -Recurse -Force }
if (Test-Path -LiteralPath $distRoot) { Remove-Item -LiteralPath $distRoot -Recurse -Force }
New-Item -ItemType Directory -Path $buildDirectory, $packageDirectory | Out-Null

Push-Location $buildDirectory
try {
    & $qmake (Join-Path $projectRoot "AudioInspectorQt.pro")
    if ($LASTEXITCODE -ne 0) { throw "QMake configuration failed." }
    & $make -j4
    if ($LASTEXITCODE -ne 0) { throw "Release compilation failed." }
} finally {
    Pop-Location
}

$executable = Join-Path $buildDirectory "AudioInspectorQt.exe"
if (-not (Test-Path -LiteralPath $executable)) { throw "Release executable was not generated." }
Copy-Item -LiteralPath $executable -Destination $packageDirectory

& $deploy --release --compiler-runtime --no-translations --no-system-d3d-compiler `
    (Join-Path $packageDirectory "AudioInspectorQt.exe")
if ($LASTEXITCODE -ne 0) { throw "Qt deployment failed." }

Copy-Item -LiteralPath (Join-Path $projectRoot "README.md") -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $projectRoot "LICENSE") -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $projectRoot "TRADEMARKS.md") -Destination $packageDirectory

Compress-Archive -LiteralPath $packageDirectory -DestinationPath $zipPath -CompressionLevel Optimal
Write-Host "Portable package: $packageDirectory"
Write-Host "ZIP package:      $zipPath"
