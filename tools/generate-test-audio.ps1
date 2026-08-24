param(
    [string]$OutputDirectory = (Join-Path $PSScriptRoot "..\test-audio"),
    [switch]$MusicalDemoOnly,
    [string]$MusicalOutputName = "demo-track-clean.wav",
    [switch]$FaultDemoOnly
)

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
    public enum Signal { Clean, Clipped, Square, DcOffset, SilentRight, Silence, LowRate, MusicalDemo, FaultDemo }

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
                    double envelope = (signal == Signal.MusicalDemo || signal == Signal.FaultDemo)
                        ? MusicalEnvelope(t, seconds) : Envelope(t, seconds);
                    double value = Sample(signal, t, channel) * envelope;
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

    private static double MusicalEnvelope(double t, double seconds)
    {
        double fadeIn = Math.Min(1.0, t / .35);
        double fadeOut = Math.Min(1.0, Math.Max(0.0, (seconds - t) / 1.8));
        return fadeIn * fadeIn * (3.0 - 2.0 * fadeIn) *
               fadeOut * fadeOut * (3.0 - 2.0 * fadeOut);
    }

    private static double Noise(int sample)
    {
        // Deterministic pseudo-noise: the generated demo is reproducible.
        double x = Math.Sin(sample * 12.9898 + 78.233) * 43758.5453;
        return 2.0 * (x - Math.Floor(x)) - 1.0;
    }

    private static double MusicalSample(double t, int channel)
    {
        const double beat = .5; // 120 BPM
        double beatPhase = t % beat;
        int beatIndex = (int)Math.Floor(t / beat);
        int bar = beatIndex / 4;

        // C, A, F and G roots. This neutral progression is only raw synthesis,
        // not a quotation or recreation of an existing recording.
        double[] roots = { 65.406, 55.000, 43.654, 48.999 };
        double root = roots[bar % roots.Length];
        double[] ratios = (bar % 4 == 1)
            ? new double[] { 1.0, 1.189207, 1.498307 }
            : new double[] { 1.0, 1.259921, 1.498307 };

        // Soft stereo chord bed with a slight, intentional channel difference.
        double pad = 0.0;
        for (int i = 0; i < ratios.Length; ++i) {
            double frequency = root * 4.0 * ratios[i] * (channel == 0 ? .9995 : 1.0005);
            pad += Math.Sin(2.0 * Math.PI * frequency * t + channel * .18 * i);
        }
        pad *= .105 * (.82 + .18 * Math.Sin(2.0 * Math.PI * .125 * t));

        // Plucked bass, kick, snare and hi-hat create visible transients and a
        // useful broadband spectrum without samples or external recordings.
        double bass = .13 * Math.Sin(2.0 * Math.PI * root * 2.0 * t) * Math.Exp(-2.6 * beatPhase);
        double kick = .22 * Math.Sin(2.0 * Math.PI * (54.0 + 38.0 * Math.Exp(-24.0 * beatPhase)) * beatPhase)
                      * Math.Exp(-15.0 * beatPhase);
        double snarePhase = beatPhase;
        double snare = (beatIndex % 4 == 1 || beatIndex % 4 == 3)
            ? .11 * Noise((int)(t * 44100.0)) * Math.Exp(-18.0 * snarePhase) : 0.0;
        double hatPhase = t % .25;
        double hat = .050 * Noise((int)(t * 88200.0) + channel * 17) * Math.Exp(-42.0 * hatPhase);

        // Short pentatonic lead notes alternate between channels to make the
        // stereo correlation and mono compatibility measurements meaningful.
        double[] melody = { 1.0, 1.122462, 1.259921, 1.498307, 1.681793, 1.498307, 1.259921, 1.122462 };
        int step = (int)Math.Floor(t / .25);
        double notePhase = t % .25;
        double leadFrequency = root * 8.0 * melody[(step + bar) % melody.Length];
        double pan = ((step & 1) == channel) ? .14 : .060;
        double lead = pan * Math.Sin(2.0 * Math.PI * leadFrequency * t) * Math.Exp(-7.0 * notePhase);

        // Leave deliberate conversion headroom. The demo should exercise the
        // analyzer without triggering a true-peak or clipping warning itself.
        return .94 * (pad + bass + kick + snare + hat + lead);
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
            case Signal.MusicalDemo: return MusicalSample(t, channel);
            case Signal.FaultDemo: {
                // A fictional "bad rip": the original musical synthesis is
                // overdriven by changing amounts, shifted by DC and polluted
                // with subsonic energy. No external recording is involved.
                double sectionGain = ((t > 6.0 && t < 9.0) || (t > 15.0 && t < 18.5)) ? 2.35 : 1.72;
                double music = sectionGain * MusicalSample(t, channel);
                double subsonic = .17 * Math.Sin(2 * Math.PI * 9 * t + channel * 1.15);
                double dc = channel == 0 ? .085 : -.060;
                return music + subsonic + dc;
            }
            default: return 0;
        }
    }
}
'@

$signals = [TestAudioGenerator+Signal]
if (-not $MusicalDemoOnly -and -not $FaultDemoOnly) {
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "01-clean-reference.wav"),       44100, 2, 2, $signals::Clean)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "02-hard-clipping.wav"),        44100, 2, 2, $signals::Clipped)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "03-full-scale-square.wav"),    44100, 1, 2, $signals::Square)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "04-dc-offset.wav"),            44100, 1, 2, $signals::DcOffset)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "05-silent-right-channel.wav"), 44100, 2, 2, $signals::SilentRight)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "06-digital-silence.wav"),      44100, 2, 2, $signals::Silence)
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "07-low-sample-rate.wav"),      22050, 1, 2, $signals::LowRate)
}
if (-not $FaultDemoOnly) {
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput $MusicalOutputName), 44100, 2, 24, $signals::MusicalDemo)
}
if ($FaultDemoOnly) {
    [TestAudioGenerator]::Write((Join-Path $resolvedOutput "demo-track-bad-rip.wav"), 44100, 2, 24, $signals::FaultDemo)
}

Write-Host "Test audio created in $resolvedOutput"
