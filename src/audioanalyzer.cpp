#include "audioanalyzer.h"
#include "dspanalysis.h"

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QFileInfo>
#include <QUrl>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace {
// Convierte cualquier formato PCM que entregue Qt a una amplitud 0..1.
double bufferPeak(const QAudioBuffer &buffer)
{
    const QAudioFormat format = buffer.format();
    const int count = buffer.sampleCount();
    double peak = 0.0;

    switch (format.sampleFormat()) {
    case QAudioFormat::UInt8: {
        const auto *samples = buffer.constData<quint8>();
        for (int i = 0; i < count; ++i)
            peak = std::max(peak, std::abs((static_cast<int>(samples[i]) - 128) / 128.0));
        break;
    }
    case QAudioFormat::Int16: {
        const auto *samples = buffer.constData<qint16>();
        for (int i = 0; i < count; ++i)
            peak = std::max(peak, std::abs(samples[i] / 32768.0));
        break;
    }
    case QAudioFormat::Int32: {
        const auto *samples = buffer.constData<qint32>();
        for (int i = 0; i < count; ++i)
            peak = std::max(peak, std::abs(samples[i] / 2147483648.0));
        break;
    }
    case QAudioFormat::Float: {
        const auto *samples = buffer.constData<float>();
        for (int i = 0; i < count; ++i)
            peak = std::max(peak, std::abs(static_cast<double>(samples[i])));
        break;
    }
    default:
        break;
    }
    return peak;
}

// Estima picos entre muestras interpolando cada intervalo a 4x. Es una
// estimacion practica, no un medidor ITU-R BS.1770 certificado.
void updateTruePeak(const QAudioBuffer &buffer, double &maximum,
                    double leftHistory[4], double rightHistory[4], int &historyCount)
{
    const QAudioFormat format = buffer.format();
    const int channels = qMax(1, format.channelCount());
    auto sample = [&](int i) -> double {
        switch (format.sampleFormat()) {
        case QAudioFormat::UInt8: return (buffer.constData<quint8>()[i] - 128) / 128.0;
        case QAudioFormat::Int16: return buffer.constData<qint16>()[i] / 32768.0;
        case QAudioFormat::Int32: return buffer.constData<qint32>()[i] / 2147483648.0;
        case QAudioFormat::Float: return buffer.constData<float>()[i];
        default: return 0.0;
        }
    };
    auto interpolate = [&](double history[4]) {
        for (int phase = 0; phase <= 4; ++phase) {
            const double t = phase / 4.0;
            const double value = .5 * ((2 * history[1])
                + (-history[0] + history[2]) * t
                + (2 * history[0] - 5 * history[1] + 4 * history[2] - history[3]) * t * t
                + (-history[0] + 3 * history[1] - 3 * history[2] + history[3]) * t * t * t);
            maximum = std::max(maximum, std::abs(value));
        }
    };
    const int frames = buffer.sampleCount() / channels;
    for (int frame = 0; frame < frames; ++frame) {
        for (int i = 0; i < 3; ++i) {
            leftHistory[i] = leftHistory[i + 1];
            rightHistory[i] = rightHistory[i + 1];
        }
        leftHistory[3] = sample(frame * channels);
        rightHistory[3] = channels > 1 ? sample(frame * channels + 1) : leftHistory[3];
        if (historyCount < 4) ++historyCount;
        if (historyCount == 4) { interpolate(leftHistory); interpolate(rightHistory); }
    }
}

void appendEnvelope(const QAudioBuffer &buffer,
                    QVector<float> &leftMinimums, QVector<float> &leftMaximums,
                    QVector<float> &rightMinimums, QVector<float> &rightMaximums,
                    int &stride, int &pendingFrames,
                    float &pendingLeftMinimum, float &pendingLeftMaximum,
                    float &pendingRightMinimum, float &pendingRightMaximum)
{
    // La envolvente y la traza son productos diferentes: esta función conserva
    // extremos para las vistas alejadas, mientras appendAnalysisSignal guarda
    // una señal temporal reducida para DSP y zoom de osciloscopio.
    const QAudioFormat format = buffer.format();
    const int channels = qMax(format.channelCount(), 1);
    const int frames = buffer.sampleCount() / channels;
    auto sample = [&](int i) -> float {
        switch (format.sampleFormat()) {
        case QAudioFormat::UInt8: return (buffer.constData<quint8>()[i] - 128) / 128.0f;
        case QAudioFormat::Int16: return buffer.constData<qint16>()[i] / 32768.0f;
        case QAudioFormat::Int32: return buffer.constData<qint32>()[i] / 2147483648.0f;
        case QAudioFormat::Float: return buffer.constData<float>()[i];
        default: return 0.0f;
        }
    };
    // Una entrada por muestra permite que el zoom muestre una traza auténtica
    // como la de un osciloscopio. En archivos largos se compacta más abajo.
    for (int frame = 0; frame < frames; ++frame) {
        const float left = sample(frame * channels);
        const float right = channels > 1 ? sample(frame * channels + 1) : left;
        pendingLeftMinimum = qMin(pendingLeftMinimum, left);
        pendingLeftMaximum = qMax(pendingLeftMaximum, left);
        pendingRightMinimum = qMin(pendingRightMinimum, right);
        pendingRightMaximum = qMax(pendingRightMaximum, right);
        if (++pendingFrames >= stride) {
            leftMinimums.append(pendingLeftMinimum);
            leftMaximums.append(pendingLeftMaximum);
            rightMinimums.append(pendingRightMinimum);
            rightMaximums.append(pendingRightMaximum);
            pendingFrames = 0;
            pendingLeftMinimum = pendingRightMinimum = 1.0f;
            pendingLeftMaximum = pendingRightMaximum = -1.0f;
        }
    }
    if (leftMinimums.size() > 500000) {
        // Compactación progresiva: cada par de intervalos se fusiona sin perder
        // su mínimo ni su máximo. Así la memoria no crece con toda la canción.
        const int newSize = leftMinimums.size() / 2;
        for (int i = 0; i < newSize; ++i) {
            leftMinimums[i] = qMin(leftMinimums[i * 2], leftMinimums[i * 2 + 1]);
            leftMaximums[i] = qMax(leftMaximums[i * 2], leftMaximums[i * 2 + 1]);
            rightMinimums[i] = qMin(rightMinimums[i * 2], rightMinimums[i * 2 + 1]);
            rightMaximums[i] = qMax(rightMaximums[i * 2], rightMaximums[i * 2 + 1]);
        }
        leftMinimums.resize(newSize); leftMaximums.resize(newSize);
        rightMinimums.resize(newSize); rightMaximums.resize(newSize);
        stride *= 2;
    }
}

void accumulateStatistics(const QAudioBuffer &buffer, double &leftSum, double &rightSum,
                          double &leftSquareSum, double &rightSquareSum,
                          qint64 &analyzedFrames, qint64 &silentFrames, qint64 &clippedFrames)
{
    // Acumuladores de una sola pasada. Evitan almacenar PCM completo para RMS,
    // componente DC, porcentaje de silencio y número de frames recortados.
    const QAudioFormat format = buffer.format();
    const int channels = qMax(1, format.channelCount());
    auto sample = [&](int i) -> double {
        switch (format.sampleFormat()) {
        case QAudioFormat::UInt8: return (buffer.constData<quint8>()[i] - 128) / 128.0;
        case QAudioFormat::Int16: return buffer.constData<qint16>()[i] / 32768.0;
        case QAudioFormat::Int32: return buffer.constData<qint32>()[i] / 2147483648.0;
        case QAudioFormat::Float: return buffer.constData<float>()[i];
        default: return 0.0;
        }
    };
    const int frames = buffer.sampleCount() / channels;
    for (int frame = 0; frame < frames; ++frame) {
        const double left = sample(frame * channels);
        const double right = channels > 1 ? sample(frame * channels + 1) : left;
        leftSum += left; rightSum += right;
        leftSquareSum += left * left; rightSquareSum += right * right;
        if (std::abs(left) < .0001 && std::abs(right) < .0001) ++silentFrames;
        if (std::abs(left) >= .9999 || std::abs(right) >= .9999) ++clippedFrames;
    }
    analyzedFrames += frames;
}

void appendAnalysisSignal(const QAudioBuffer &buffer, QVector<float> &leftOutput,
                          QVector<float> &rightOutput, int &stride, int &phase,
                          int &outputRate)
{
    // Reducimos aproximadamente a 12 kHz. Es suficiente para tempo, tonalidad
    // y bandas generales, y evita que varios minutos ocupen cientos de MB.
    const QAudioFormat format = buffer.format();
    const int channels = qMax(1, format.channelCount());
    if (outputRate == 0) {
        stride = qMax(1, format.sampleRate() / 12000);
        outputRate = format.sampleRate() / stride;
    }
    auto sample = [&](int i) -> float {
        switch (format.sampleFormat()) {
        case QAudioFormat::UInt8: return (buffer.constData<quint8>()[i] - 128) / 128.0f;
        case QAudioFormat::Int16: return buffer.constData<qint16>()[i] / 32768.0f;
        case QAudioFormat::Int32: return buffer.constData<qint32>()[i] / 2147483648.0f;
        case QAudioFormat::Float: return buffer.constData<float>()[i];
        default: return 0.0f;
        }
    };
    const int frames = buffer.sampleCount() / channels;
    for (int frame = 0; frame < frames; ++frame) {
        if (phase++ % stride != 0) continue;
        const float left = sample(frame * channels);
        leftOutput.append(left);
        rightOutput.append(channels > 1 ? sample(frame * channels + 1) : left);
    }
}

int bitsForFormat(QAudioFormat::SampleFormat format)
{
    switch (format) {
    case QAudioFormat::UInt8: return 8;
    case QAudioFormat::Int16: return 16;
    case QAudioFormat::Int32:
    case QAudioFormat::Float: return 32;
    default: return 0;
    }
}
}

AudioAnalyzer::AudioAnalyzer(QObject *parent)
    : QObject(parent), decoder_(new QAudioDecoder(this))
{
    // QAudioDecoder entrega buffers de forma asíncrona: la interfaz permanece
    // operativa mientras se analiza un archivo largo.
    connect(decoder_, &QAudioDecoder::bufferReady, this, &AudioAnalyzer::consumeBuffer);
    connect(decoder_, &QAudioDecoder::finished, this, [this] { finishCurrent(); });
    connect(decoder_, qOverload<QAudioDecoder::Error>(&QAudioDecoder::error), this,
            [this](QAudioDecoder::Error) { finishCurrent(decoder_->errorString()); });
}

void AudioAnalyzer::enqueue(const QStringList &paths)
{
    for (const QString &path : paths)
        if (!queue_.contains(path) && path != currentPath_) queue_.append(path);
    if (currentPath_.isEmpty()) startNext();
}

void AudioAnalyzer::analyzeFile(const QString &path)
{
    // Semántica de archivo único: cancelar el trabajo anterior garantiza que
    // nunca se mezclen medidas de dos selecciones distintas.
    queue_.clear();
    currentPath_.clear();
    decoder_->stop();
    if (!path.isEmpty()) {
        queue_.append(path);
        startNext();
    }
}

void AudioAnalyzer::startNext()
{
    if (queue_.isEmpty()) return;
    currentPath_ = queue_.takeFirst();
    // Reinicio exhaustivo de todos los acumuladores antes de iniciar el decoder.
    sampleRate_ = bits_ = channels_ = 0;
    durationUs_ = 0;
    maximumPeak_ = 0.0;
    maximumTruePeak_ = 0.0;
    std::fill(std::begin(truePeakLeftHistory_), std::end(truePeakLeftHistory_), 0.0);
    std::fill(std::begin(truePeakRightHistory_), std::end(truePeakRightHistory_), 0.0);
    truePeakHistoryCount_ = 0;
    leftMinimums_.clear(); leftMaximums_.clear();
    rightMinimums_.clear(); rightMaximums_.clear();
    leftSum_ = rightSum_ = leftSquareSum_ = rightSquareSum_ = 0.0;
    analyzedFrames_ = silentFrames_ = clippedFrames_ = 0;
    envelopeStride_ = 1;
    pendingEnvelopeFrames_ = 0;
    pendingLeftMinimum_ = pendingRightMinimum_ = 1.0f;
    pendingLeftMaximum_ = pendingRightMaximum_ = -1.0f;
    analysisLeft_.clear(); analysisRight_.clear();
    analysisStride_ = 1; analysisPhase_ = 0; analysisSampleRate_ = 0;
    decoder_->setSource(QUrl::fromLocalFile(currentPath_));
    decoder_->start();
}

void AudioAnalyzer::consumeBuffer()
{
    const QAudioBuffer buffer = decoder_->read();
    if (!buffer.isValid()) return;
    const QAudioFormat format = buffer.format();
    sampleRate_ = format.sampleRate();
    channels_ = format.channelCount();
    bits_ = bitsForFormat(format.sampleFormat());
    durationUs_ += buffer.duration();
    // Cada buffer actualiza cuatro representaciones complementarias: pico,
    // true peak, envolvente visual, estadísticas globales y señal DSP reducida.
    maximumPeak_ = std::max(maximumPeak_, bufferPeak(buffer));
    updateTruePeak(buffer, maximumTruePeak_, truePeakLeftHistory_,
                   truePeakRightHistory_, truePeakHistoryCount_);
    appendEnvelope(buffer, leftMinimums_, leftMaximums_, rightMinimums_, rightMaximums_,
                   envelopeStride_, pendingEnvelopeFrames_,
                   pendingLeftMinimum_, pendingLeftMaximum_,
                   pendingRightMinimum_, pendingRightMaximum_);
    accumulateStatistics(buffer, leftSum_, rightSum_, leftSquareSum_, rightSquareSum_,
                         analyzedFrames_, silentFrames_, clippedFrames_);
    appendAnalysisSignal(buffer, analysisLeft_, analysisRight_, analysisStride_,
                         analysisPhase_, analysisSampleRate_);
}

void AudioAnalyzer::finishCurrent(const QString &error)
{
    if (currentPath_.isEmpty()) return;
    const QString path = currentPath_;
    currentPath_.clear();
    decoder_->stop();

    // La tasa de bits se deduce de tamaño/duración. Para formatos variables es
    // una media del archivo, no el bitrate instantáneo del codificador.
    const qint64 durationMs = durationUs_ / 1000;
    const qint64 bytes = QFileInfo(path).size();
    const int bitrate = durationMs > 0
        ? qRound((bytes * 8.0) / durationMs)
        : 0;
    const double peakDb = maximumPeak_ > 0.0
        ? 20.0 * std::log10(maximumPeak_)
        : -INFINITY;
    const auto toDb = [](double value) {
        return value > 0.0 ? 20.0 * std::log10(value) : -INFINITY;
    };
    const double leftRmsDb = toDb(analyzedFrames_ > 0
        ? std::sqrt(leftSquareSum_ / analyzedFrames_) : 0.0);
    const double rightRmsDb = toDb(analyzedFrames_ > 0
        ? std::sqrt(rightSquareSum_ / analyzedFrames_) : 0.0);
    DspAnalysis dsp = analyzeDsp(analysisLeft_, analysisRight_, analysisSampleRate_);
    dsp.truePeakDbtp = maximumTruePeak_ > 0.0
        ? 20.0 * std::log10(maximumTruePeak_) : -INFINITY;
    // Se emite un único resultado coherente. QVector usa copia implícita en Qt,
    // por lo que transportar las gráficas no duplica inmediatamente su memoria.
    emit analysisFinished(path, sampleRate_, bits_, channels_, durationMs,
                          bitrate, peakDb, maximumPeak_ >= 0.9999, error,
                          leftMinimums_, leftMaximums_, rightMinimums_, rightMaximums_,
                          analysisLeft_, analysisRight_,
                          leftRmsDb, rightRmsDb,
                          analyzedFrames_ > 0 ? leftSum_ / analyzedFrames_ : 0.0,
                          analyzedFrames_ > 0 ? rightSum_ / analyzedFrames_ : 0.0,
                          analyzedFrames_ > 0 ? silentFrames_ * 100.0 / analyzedFrames_ : 0.0,
                          clippedFrames_, dsp);
    startNext();
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
