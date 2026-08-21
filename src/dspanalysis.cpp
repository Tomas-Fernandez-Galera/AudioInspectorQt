#include "dspanalysis.h"
#include <QtMath>
#include <algorithm>
#include <cmath>
#include <complex>
#include <QPainter>

namespace {
using Complex = std::complex<double>;

// FFT radix-2 iterativa, implementada aquí para no introducir una dependencia
// externa solo para el análisis espectral básico.
void fft(QVector<Complex> &data)
{
    const int n = data.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(data[i], data[j]);
    }
    for (int length = 2; length <= n; length <<= 1) {
        const Complex step = std::polar(1.0, -2.0 * M_PI / length);
        for (int start = 0; start < n; start += length) {
            Complex phase(1.0, 0.0);
            for (int j = 0; j < length / 2; ++j) {
                const Complex even = data[start + j];
                const Complex odd = data[start + j + length / 2] * phase;
                data[start + j] = even + odd;
                data[start + j + length / 2] = even - odd;
                phase *= step;
            }
        }
    }
}

double powerToLufs(double power)
{
    // La constante aproxima la referencia BS.1770. Falta ponderación K completa,
    // por eso la interfaz identifica siempre estos valores como estimaciones.
    return power > 1e-14 ? -0.691 + 10.0 * std::log10(power) : -INFINITY;
}

double percentile(QVector<double> values, double fraction)
{
    if (values.isEmpty()) return 0.0;
    std::sort(values.begin(), values.end());
    return values[qBound(0, qRound((values.size() - 1) * fraction), values.size() - 1)];
}
}

DspAnalysis analyzeDsp(const QVector<float> &left, const QVector<float> &right, int sampleRate)
{
    DspAnalysis result;
    const int count = qMin(left.size(), right.size());
    if (count < 2048 || sampleRate <= 0) return result;
    // La suma mono alimenta las medidas que no necesitan conservar información
    // lateral. La señal estéreo original se mantiene para correlación y RMS.
    QVector<float> mono(count);
    double sumL2 = 0, sumR2 = 0, sumLR = 0, monoPower = 0;
    for (int i = 0; i < count; ++i) {
        mono[i] = (left[i] + right[i]) * .5f;
        sumL2 += left[i] * left[i]; sumR2 += right[i] * right[i];
        sumLR += left[i] * right[i]; monoPower += mono[i] * mono[i];
    }
    result.correlation = sumLR / std::sqrt(qMax(1e-20, sumL2 * sumR2));
    const double stereoPower = (sumL2 + sumR2) / (2.0 * count);
    result.monoCompatibilityDb = stereoPower > 0
        ? 10.0 * std::log10((monoPower / count) / stereoPower) : 0.0;

    // Ignoramos silencios menores de 50 ms para no informar pequeños márgenes
    // propios de la edición o de la codificación con pérdida.
    const int silenceThreshold = qRound(sampleRate * .05);
    int leading = 0, trailing = 0;
    while (leading < count && std::abs(mono[leading]) < .001f) ++leading;
    while (trailing < count && std::abs(mono[count - 1 - trailing]) < .001f) ++trailing;
    result.leadingSilenceSeconds = leading >= silenceThreshold ? leading / double(sampleRate) : 0.0;
    result.trailingSilenceSeconds = trailing >= silenceThreshold ? trailing / double(sampleRate) : 0.0;

    // Ventanas de tres segundos para sonoridad y dinámica. En archivos más
    // cortos se usa una única ventana con toda la señal, evitando -infinito.
    QVector<double> blockPowers;
    const int block = qMin(count, qMax(1, sampleRate * 3));
    for (int start = 0; start < count; start += block) {
        const int blockLength = qMin(block, count - start);
        double power = 0;
        for (int i = start; i < start + blockLength; ++i) power += mono[i] * mono[i];
        blockPowers.append(power / blockLength);
    }
    QVector<double> gated;
    for (double power : blockPowers) if (powerToLufs(power) > -70.0) gated.append(power);
    double initialPower = 0;
    for (double p : gated) initialPower += p;
    if (!gated.isEmpty()) initialPower /= gated.size();
    const double relativeGate = powerToLufs(initialPower) - 10.0;
    double integratedPower = 0; int integratedBlocks = 0;
    QVector<double> sectionDb;
    for (double power : gated) {
        const double level = powerToLufs(power);
        sectionDb.append(level);
        if (level >= relativeGate) { integratedPower += power; ++integratedBlocks; }
    }
    result.integratedLufs = powerToLufs(integratedBlocks ? integratedPower / integratedBlocks : 0);
    result.dynamicRangeDb = percentile(sectionDb, .95) - percentile(sectionDb, .10);
    result.quietSectionLufs = percentile(sectionDb, .10);
    result.medianSectionLufs = percentile(sectionDb, .50);
    result.loudSectionLufs = percentile(sectionDb, .95);
    // Recorre ventanas solapadas cada 100 ms y devuelve la más intensa. Es más
    // representativo en un informe estático que medir solo el final del archivo.
    auto maximumWindowLufs = [&](double seconds) -> double {
        const int samples = qMin(count, qRound(sampleRate * seconds));
        if (samples <= 0) return -INFINITY;
        double power = 0;
        for (int i = 0; i < samples; ++i) power += mono[i] * mono[i];
        double maximumPower = power;
        const int hopSamples = qMax(1, sampleRate / 10);
        for (int start = hopSamples; start + samples <= count; start += hopSamples) {
            for (int i = start - hopSamples; i < start; ++i) power -= mono[i] * mono[i];
            for (int i = start + samples - hopSamples; i < start + samples; ++i)
                power += mono[i] * mono[i];
            maximumPower = qMax(maximumPower, power);
        }
        return powerToLufs(maximumPower / samples);
    };
    result.momentaryLufs = maximumWindowLufs(.4);
    result.shortTermLufs = maximumWindowLufs(3.0);

    // FFT con 50 % de solapamiento: compromiso entre resolución temporal,
    // resolución frecuencial y coste para archivos musicales largos.
    constexpr int fftSize = 4096, hop = 2048;
    result.spectrogram = QImage(512, 120, QImage::Format_RGB32);
    result.spectrogram.fill(QColor("#090d15"));
    QVector<float> spectrogramDb(result.spectrogram.width() * result.spectrogram.height(), -120.0f);
    double sub = 0, low = 0, mid = 0, high = 0;
    QVector<double> averageSpectrum(fftSize / 2, 0.0);
    int spectrumFrames = 0;
    QVector<double> chroma(12, 0.0), onset;
    double previousEnergy = 0;
    for (int start = 0; start + fftSize <= count; start += hop) {
        QVector<Complex> spectrum(fftSize);
        double energy = 0;
        for (int i = 0; i < fftSize; ++i) {
            const double window = .5 - .5 * std::cos(2 * M_PI * i / (fftSize - 1));
            spectrum[i] = mono[start + i] * window;
            energy += mono[start + i] * mono[start + i];
        }
        onset.append(qMax(0.0, energy - previousEnergy)); previousEnergy = energy;
        fft(spectrum);
        ++spectrumFrames;
        const int column = qBound(0, int(qint64(start) * result.spectrogram.width()
            / qMax(1, count - fftSize)), result.spectrogram.width() - 1);
        for (int y = 0; y < result.spectrogram.height(); ++y) {
            const double fraction = 1.0 - y / double(result.spectrogram.height() - 1);
            const double frequency = 20.0 * std::pow((sampleRate / 2.0) / 20.0, fraction);
            const int bin = qBound(1, qRound(frequency * fftSize / sampleRate), fftSize / 2 - 1);
            const double normalizedPower = std::norm(spectrum[bin])
                / double(fftSize * fftSize);
            const double db = 10.0 * std::log10(normalizedPower + 1e-14);
            spectrogramDb[y * result.spectrogram.width() + column] = float(db);
        }
        for (int bin = 1; bin < fftSize / 2; ++bin) {
            const double frequency = bin * sampleRate / double(fftSize);
            const double power = std::norm(spectrum[bin]) / double(fftSize * fftSize);
            averageSpectrum[bin] += power;
            if (frequency < 20) sub += power;
            else if (frequency < 250) low += power;
            else if (frequency < 4000) mid += power;
            else high += power;
            if (frequency >= 55 && frequency <= 5000) {
                const int midi = qRound(69 + 12 * std::log2(frequency / 440.0));
                chroma[(midi % 12 + 12) % 12] += std::sqrt(power);
            }
        }
    }
    const double spectralTotal = qMax(1e-20, sub + low + mid + high);
    result.subsonicPercent = sub * 100 / spectralTotal;
    result.lowPercent = low * 100 / spectralTotal;
    result.midPercent = mid * 100 / spectralTotal;
    result.highPercent = high * 100 / spectralTotal;

    // La escala de color se adapta al contenido: el pico visible ocupa la
    // parte alta y se muestran 70 dB por debajo sin dejar audios suaves negros.
    const float spectrogramCeiling = *std::max_element(spectrogramDb.cbegin(), spectrogramDb.cend());
    const float spectrogramFloor = spectrogramCeiling - 70.0f;
    for (int y = 0; y < result.spectrogram.height(); ++y) {
        for (int x = 0; x < result.spectrogram.width(); ++x) {
            const float db = spectrogramDb[y * result.spectrogram.width() + x];
            const double intensity = qBound(0.0,
                double(db - spectrogramFloor) / 70.0, 1.0);
            const QColor color = QColor::fromHsvF(.66 - .66 * intensity,
                                                   .92, .06 + .94 * intensity);
            result.spectrogram.setPixelColor(x, y, color);
        }
    }

    // Curva FFT media: eje de frecuencia logaritmico de 20 Hz a Nyquist.
    result.spectrumPlot = QImage(512, 120, QImage::Format_RGB32);
    result.spectrumPlot.fill(QColor("#090d15"));
    QPainter spectrumPainter(&result.spectrumPlot);
    spectrumPainter.setRenderHint(QPainter::Antialiasing, true);
    spectrumPainter.setPen(QPen(QColor("#26364f"), 1));
    for (int x = 0; x < result.spectrumPlot.width(); x += 64)
        spectrumPainter.drawLine(x, 0, x, result.spectrumPlot.height());
    for (int y = 0; y < result.spectrumPlot.height(); y += 30)
        spectrumPainter.drawLine(0, y, result.spectrumPlot.width(), y);
    QPolygonF curve;
    const double maximumFrequency = sampleRate / 2.0;
    for (int x = 0; x < result.spectrumPlot.width(); ++x) {
        const double fraction = x / double(result.spectrumPlot.width() - 1);
        const double frequency = 20.0 * std::pow(maximumFrequency / 20.0, fraction);
        const double exactBin = frequency * fftSize / sampleRate;
        const int lowerBin = qBound(1, int(std::floor(exactBin)), fftSize / 2 - 2);
        const double blend = qBound(0.0, exactBin - lowerBin, 1.0);
        const double power = ((1.0 - blend) * averageSpectrum[lowerBin]
            + blend * averageSpectrum[lowerBin + 1]) / qMax(1, spectrumFrames);
        const double db = 10.0 * std::log10(power + 1e-14);
        const double normalized = (qBound(-80.0, db, 0.0) + 80.0) / 80.0;
        curve.append(QPointF(x, (1.0 - normalized) * (result.spectrumPlot.height() - 1)));
    }
    spectrumPainter.setPen(QPen(QColor("#2f82ff"), 1.6));
    spectrumPainter.drawPolyline(curve);

    // El tempo se obtiene autocorrelacionando una envolvente de ataques. No se
    // ofrece para menos de cuatro segundos porque el resultado sería engañoso.
    double bestCorrelation = 0;
    int bestLag = 0, bestBpm = 0;
    QVector<double> tempoCorrelations(201, 0.0);
    const double envelopeRate = sampleRate / double(hop);
    for (int bpm = 60; bpm <= 200 && count >= sampleRate * 4; ++bpm) {
        const int lag = qRound(envelopeRate * 60.0 / bpm);
        double correlation = 0, norm = 0;
        for (int i = lag; i < onset.size(); ++i) {
            correlation += onset[i] * onset[i - lag]; norm += onset[i] * onset[i];
        }
        correlation /= qMax(1e-20, norm);
        tempoCorrelations[bpm] = correlation;
        if (correlation > bestCorrelation) {
            bestCorrelation = correlation; bestLag = lag; bestBpm = bpm;
        }
    }
    double secondCorrelation = 0;
    for (int bpm = 60; bpm <= 200; ++bpm)
        if (std::abs(bpm - bestBpm) > 4)
            secondCorrelation = qMax(secondCorrelation, tempoCorrelations[bpm]);
    if (bestLag > 0) result.bpm = 60.0 * envelopeRate / bestLag;
    QVector<double> validTempoCorrelations;
    for (int bpm = 60; bpm <= 200; ++bpm)
        validTempoCorrelations.append(tempoCorrelations[bpm]);
    const double typicalCorrelation = percentile(validTempoCorrelations, .50);
    const double prominence = (bestCorrelation - typicalCorrelation)
        / qMax(1e-9, bestCorrelation);
    const double separation = (bestCorrelation - secondCorrelation)
        / qMax(1e-9, bestCorrelation);
    result.bpmConfidence = qBound(0.0,
        100.0 * (.75 * prominence + .25 * qMax(0.0, separation)), 100.0);

    // Perfiles de Krumhansl: comparamos la energía de las doce clases tonales
    // con cada raíz mayor/menor y conservamos la puntuación más alta.
    const double majorProfile[12] = {6.35,2.23,3.48,2.33,4.38,4.09,2.52,5.19,2.39,3.66,2.29,2.88};
    const double minorProfile[12] = {6.33,2.68,3.52,5.38,2.60,3.53,2.54,4.75,3.98,2.69,3.34,3.17};
    const char *names[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    double bestKey = -1; int bestRoot = 0; bool minor = false;
    for (int root = 0; root < 12; ++root) for (int mode = 0; mode < 2; ++mode) {
        double score = 0; const double *profile = mode ? minorProfile : majorProfile;
        for (int note = 0; note < 12; ++note) score += chroma[(note + root) % 12] * profile[note];
        if (score > bestKey) { bestKey = score; bestRoot = root; minor = mode; }
    }
    result.musicalKey = QStringLiteral("%1 %2").arg(names[bestRoot], minor ? "minor" : "major");
    return result;
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
