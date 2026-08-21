# Audio format test set

Every file contains the same original synthetic stereo signal: low-level start,
smooth rise, steady middle and fade-out. No copyrighted recording is included.

The set covers FLAC, MP3, OGG Vorbis, Opus, M4A/AAC, raw AAC, WMA and AIFF.
Lossy formats can produce slightly different peak readings after decoding; that
is expected and useful for testing AudioInspector Qt.

Regenerate the set with a local FFmpeg executable:

```powershell
powershell -ExecutionPolicy Bypass -File tools\generate-format-tests.ps1 -FfmpegPath "C:\path\to\ffmpeg.exe"
```
