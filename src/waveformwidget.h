#pragma once
#include <QVector>
#include <QWidget>

// Dibuja una envolvente y resalta en rojo las muestras a escala completa.
class WaveformWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit WaveformWidget(QWidget *parent = nullptr);
    void setWaveform(const QVector<float> &leftMinimums, const QVector<float> &leftMaximums,
                     const QVector<float> &rightMinimums, const QVector<float> &rightMaximums,
                     const QVector<float> &leftTrace, const QVector<float> &rightTrace,
                     int channels, qint64 durationMs);
    void clearWaveform();
    void setDarkMode(bool enabled);
    void setPlayheadPosition(qint64 milliseconds);
signals:
    void scaleWheelRequested(int steps, double filePosition);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    QVector<float> leftMinimums_, leftMaximums_;
    QVector<float> rightMinimums_, rightMaximums_;
    QVector<float> leftTrace_, rightTrace_;
    int channels_ = 0;
    qint64 durationMs_ = 0;
    bool darkMode_ = false;
    bool containsAggregatedEnvelope_ = false;
    qint64 playheadMs_ = -1;
};
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
