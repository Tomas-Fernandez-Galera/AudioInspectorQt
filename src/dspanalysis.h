#pragma once
#include <QString>
#include <QVector>
#include <QImage>

struct DspAnalysis
{
    // Sonoridad: son estimaciones internas, no resultados certificados EBU R128.
    double integratedLufs = 0.0;
    double momentaryLufs = 0.0;
    double shortTermLufs = 0.0;
    double truePeakDbtp = 0.0;
    // Estadísticas temporales calculadas sobre bloques de tres segundos.
    double dynamicRangeDb = 0.0;
    double quietSectionLufs = 0.0;
    double medianSectionLufs = 0.0;
    double loudSectionLufs = 0.0;
    double correlation = 0.0;
    double monoCompatibilityDb = 0.0;
    // Reparto relativo de energía. Las cuatro bandas suman aproximadamente 100 %.
    double subsonicPercent = 0.0;
    double lowPercent = 0.0, midPercent = 0.0, highPercent = 0.0;
    double leadingSilenceSeconds = 0.0, trailingSilenceSeconds = 0.0;
    double bpm = 0.0, bpmConfidence = 0.0;
    QString musicalKey;
    // Imágenes ya renderizadas para mantener la interfaz ajena al cálculo FFT.
    QImage spectrogram;
    QImage spectrumPlot;
};

// Recibe una copia reducida de la señal estéreo. El muestreo reducido limita
// memoria y tiempo de CPU; por ello los resultados musicales son estimaciones.
DspAnalysis analyzeDsp(const QVector<float> &left, const QVector<float> &right,
                       int sampleRate);
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
