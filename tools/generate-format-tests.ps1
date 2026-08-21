param(
    [string]$FfmpegPath = "ffmpeg",
    [string]$Source = (Join-Path $PSScriptRoot "..\test-audio\01-clean-reference.wav"),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot "..\test-audio\formats")
)

$ErrorActionPreference = "Stop"
$sourcePath = [IO.Path]::GetFullPath($Source)
$outputPath = [IO.Path]::GetFullPath($OutputDirectory)
if (-not (Test-Path -LiteralPath $sourcePath)) {
    throw "Source WAV not found: $sourcePath"
}
if (-not (Get-Command $FfmpegPath -ErrorAction SilentlyContinue)) {
    throw "FFmpeg was not found. Pass its path with -FfmpegPath."
}
[IO.Directory]::CreateDirectory($outputPath) | Out-Null

function Convert-TestAudio {
    param([string]$Name, [string[]]$Arguments)
    $destination = Join-Path $outputPath $Name
    & $FfmpegPath -hide_banner -loglevel error -y -i $sourcePath @Arguments $destination
    if ($LASTEXITCODE -ne 0) { throw "FFmpeg failed while creating $Name" }
}

Convert-TestAudio "reference.flac" @("-c:a", "flac")
Convert-TestAudio "reference.mp3"  @("-c:a", "libmp3lame", "-b:a", "192k")
Convert-TestAudio "reference.ogg"  @("-c:a", "libvorbis", "-q:a", "5")
Convert-TestAudio "reference.opus" @("-c:a", "libopus", "-b:a", "128k")
Convert-TestAudio "reference.m4a"  @("-c:a", "aac", "-b:a", "192k")
Convert-TestAudio "reference.aac"  @("-c:a", "aac", "-b:a", "192k", "-f", "adts")
Convert-TestAudio "reference.wma"  @("-c:a", "wmav2", "-b:a", "192k")
Convert-TestAudio "reference.aiff" @("-c:a", "pcm_s16be")

Write-Host "Format test files created in $outputPath"
