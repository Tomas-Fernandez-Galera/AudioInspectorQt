#pragma once

#include <QObject>
#include <QStringList>
#include <QVector>
#include "dspanalysis.h"

class QAudioBuffer;
class QAudioDecoder;

// Analiza los archivos uno a uno para no bloquear la interfaz ni mezclar
// resultados cuando el usuario añade muchos audios de una sola vez.
class AudioAnalyzer final : public QObject
{
    Q_OBJECT

public:
    explicit AudioAnalyzer(QObject *parent = nullptr);
    void enqueue(const QStringList &paths);
    void analyzeFile(const QString &path);

signals:
    // Resultado completo del archivo. Las cuatro series min/max forman la
    // envolvente; leftTrace/rightTrace contienen la señal reducida para el trazo.
    void analysisFinished(const QString &path, int sampleRate, int bits,
                          int channels, qint64 durationMs, int bitrateKbps,
                          double peakDb, bool clipping, const QString &error,
                          const QVector<float> &leftMinimums, const QVector<float> &leftMaximums,
                          const QVector<float> &rightMinimums, const QVector<float> &rightMaximums,
                          const QVector<float> &leftTrace, const QVector<float> &rightTrace,
                          double leftRmsDb, double rightRmsDb,
                          double leftDcOffset, double rightDcOffset,
                          double silencePercent, qint64 clippedFrames,
                          const DspAnalysis &dsp);

private:
    void startNext();
    void consumeBuffer();
    void finishCurrent(const QString &error = {});

    // Estado del decodificador y cola. La API conserva cola aunque la interfaz
    // actual trabaje deliberadamente con un solo archivo.
    QAudioDecoder *decoder_ = nullptr;
    QStringList queue_;
    QString currentPath_;
    int sampleRate_ = 0;
    int bits_ = 0;
    int channels_ = 0;
    qint64 durationUs_ = 0;
    double maximumPeak_ = 0.0;
    double maximumTruePeak_ = 0.0;
    double truePeakLeftHistory_[4] = {};
    double truePeakRightHistory_[4] = {};
    int truePeakHistoryCount_ = 0;
    QVector<float> leftMinimums_, leftMaximums_;
    QVector<float> rightMinimums_, rightMaximums_;
    // Estadísticas acumuladas sin conservar todas las muestras originales.
    double leftSum_ = 0.0, rightSum_ = 0.0;
    double leftSquareSum_ = 0.0, rightSquareSum_ = 0.0;
    qint64 analyzedFrames_ = 0, silentFrames_ = 0, clippedFrames_ = 0;
    int envelopeStride_ = 1;
    int pendingEnvelopeFrames_ = 0;
    float pendingLeftMinimum_ = 1.0f, pendingLeftMaximum_ = -1.0f;
    float pendingRightMinimum_ = 1.0f, pendingRightMaximum_ = -1.0f;
    // Señal reducida común al DSP y a la vista cercana del osciloscopio.
    QVector<float> analysisLeft_, analysisRight_;
    int analysisStride_ = 1, analysisPhase_ = 0, analysisSampleRate_ = 0;
};
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
