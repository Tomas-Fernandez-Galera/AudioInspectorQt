# Synthetic test audio

These WAV files are generated signals and contain no copyrighted recording.
Except for digital silence, every signal starts at low volume, rises smoothly,
holds its nominal level and finishes with a fade-out. This makes the waveform
overview and its transition into the detailed sample trace easier to verify.

- `01-clean-reference.wav`: healthy stereo sine wave near -6 dBFS.
- `02-hard-clipping.wav`: amplified sine wave with hard digital clipping.
- `03-full-scale-square.wav`: full-scale square wave.
- `04-dc-offset.wav`: signal with a strong DC offset.
- `05-silent-right-channel.wav`: stereo file with a silent right channel.
- `06-digital-silence.wav`: complete digital silence.
- `07-low-sample-rate.wav`: mono audio at only 22.05 kHz.

Regenerate the files from the project root with:

```powershell
powershell -ExecutionPolicy Bypass -File tools\generate-test-audio.ps1
```
