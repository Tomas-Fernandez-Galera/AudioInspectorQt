param([string]$OutputDirectory = (Join-Path $PSScriptRoot "..\test-audio"))

$ErrorActionPreference = "Stop"
$resolvedOutput = [IO.Path]::GetFullPath($OutputDirectory)
[IO.Directory]::CreateDirectory($resolvedOutput) | Out-Null

# El bucle de muestras se compila como C# para generar el lote rápidamente y
# sin depender de FFmpeg ni de otras herramientas externas.
Add-Type -TypeDefinition @'
using System;
using System.IO;
using System.Text;

public static class TestAudioGenerator
{
    public enum Signal { Clean, Clipped, Square, DcOffset, SilentRight, Silence, LowRate }

    public static void Write(string path, int rate, int channels, double seconds, Signal signal)
    {
        int frames = (int)(rate * seconds), dataBytes = frames * channels * 2;
        using (var writer = new BinaryWriter(File.Create(path))) {
            writer.Write(Encoding.ASCII.GetBytes("RIFF")); writer.Write(36 + dataBytes);
            writer.Write(Encoding.ASCII.GetBytes("WAVEfmt ")); writer.Write(16);
            writer.Write((short)1); writer.Write((short)channels); writer.Write(rate);
            writer.Write(rate * channels * 2); writer.Write((short)(channels * 2));
            writer.Write((short)16); writer.Write(Encoding.ASCII.GetBytes("data")); writer.Write(dataBytes);
            for (int frame = 0; frame < frames; ++frame) {
                double t = frame / (double)rate;
                for (int channel = 0; channel < channels; ++channel) {
                    double value = Sample(signal, t, channel) * Envelope(t, seconds);
                    value = Math.Max(-1.0, Math.Min(1.0, value));
                    writer.Write(value <= -1.0 ? short.MinValue : (short)Math.Round(value * 32767));
                }
            }
        }
    }

    // Comienza al 8 %, asciende suavemente hasta el nivel nominal, mantiene
    // una zona central y termina con un fade-out completo.
    private static double Envelope(double t, double seconds)
    {
        if (t < seconds * .30) {
            double p = t / (seconds * .30);
            double smooth = p * p * (3.0 - 2.0 * p);
            return .08 + .92 * smooth;
        }
        if (t > seconds * .72) {
            double p = Math.Max(0.0, (seconds - t) / (seconds * .28));
            return p * p * (3.0 - 2.0 * p);
        }
        return 1.0;
    }

    private static double Sample(Signal signal, double t, int channel)
    {
        switch (signal) {
            case Signal.Clean: return .5 * Math.Sin(2 * Math.PI * 440 * t);
            case Signal.Clipped: return 1.6 * Math.Sin(2 * Math.PI * 440 * t);
            case Signal.Square: return Math.Sin(2 * Math.PI * 220 * t) >= 0 ? 1 : -1;
            case Signal.DcOffset: return .45 + .5 * Math.Sin(2 * Math.PI * 330 * t);
            case Signal.SilentRight: return channel == 0 ? .7 * Math.Sin(2 * Math.PI * 550 * t) : 0;
            case Signal.LowRate: return .6 * Math.Sin(2 * Math.PI * 440 * t);
            default: return 0;
        }
    }
}
'@

$signals = [TestAudioGenerator+Signal]
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "01-clean-reference.wav"),       44100, 2, 2, $signals::Clean)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "02-hard-clipping.wav"),        44100, 2, 2, $signals::Clipped)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "03-full-scale-square.wav"),    44100, 1, 2, $signals::Square)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "04-dc-offset.wav"),            44100, 1, 2, $signals::DcOffset)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "05-silent-right-channel.wav"), 44100, 2, 2, $signals::SilentRight)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "06-digital-silence.wav"),      44100, 2, 2, $signals::Silence)
[TestAudioGenerator]::Write((Join-Path $resolvedOutput "07-low-sample-rate.wav"),      22050, 1, 2, $signals::LowRate)

Write-Host "Test audio created in $resolvedOutput"
