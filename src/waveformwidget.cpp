#include "waveformwidget.h"

#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QWheelEvent>

namespace {
QString timeText(qint64 milliseconds)
{
    if (milliseconds < 1000)
        return QStringLiteral("%1 s").arg(milliseconds / 1000.0, 0, 'f', 3);
    if (milliseconds < 10000)
        return QStringLiteral("%1 s").arg(milliseconds / 1000.0, 0, 'f', 2);
    const qint64 seconds = milliseconds / 1000;
    return QStringLiteral("%1:%2").arg(seconds / 60)
        .arg(seconds % 60, 2, 10, QLatin1Char('0'));
}
}

WaveformWidget::WaveformWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(190);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);
}

void WaveformWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus(Qt::MouseFocusReason);
    QWidget::mousePressEvent(event);
    update();
}

void WaveformWidget::wheelEvent(QWheelEvent *event)
{
    if (!hasFocus() || leftMinimums_.isEmpty()) {
        QWidget::wheelEvent(event);
        return;
    }
    const int steps = event->angleDelta().y() / 120;
    if (steps != 0) {
        emit scaleWheelRequested(steps,
            qBound(0.0, event->position().x() / qMax(1.0, double(width())), 1.0));
        event->accept();
    }
}

void WaveformWidget::setWaveform(const QVector<float> &leftMinimums,
                                 const QVector<float> &leftMaximums,
                                 const QVector<float> &rightMinimums,
                                 const QVector<float> &rightMaximums,
                                 const QVector<float> &leftTrace,
                                 const QVector<float> &rightTrace,
                                 int channels, qint64 durationMs)
{
    leftMinimums_ = leftMinimums; leftMaximums_ = leftMaximums;
    rightMinimums_ = rightMinimums; rightMaximums_ = rightMaximums;
    leftTrace_ = leftTrace; rightTrace_ = rightTrace;
    channels_ = channels; durationMs_ = durationMs;
    // Si minimo y maximo difieren, esta entrada ya resume varias muestras.
    // No debe convertirse en su promedio al acercar: eso reduciria los picos.
    containsAggregatedEnvelope_ = false;
    for (int i = 0; i < leftMinimums_.size(); ++i) {
        if (!qFuzzyCompare(leftMinimums_[i] + 1.0f, leftMaximums_[i] + 1.0f)
            || (channels_ > 1 && !qFuzzyCompare(rightMinimums_[i] + 1.0f,
                                                rightMaximums_[i] + 1.0f))) {
            containsAggregatedEnvelope_ = true;
            break;
        }
    }
    update();
}

void WaveformWidget::clearWaveform()
{
    leftMinimums_.clear(); leftMaximums_.clear();
    rightMinimums_.clear(); rightMaximums_.clear();
    leftTrace_.clear(); rightTrace_.clear();
    channels_ = 0; durationMs_ = 0; containsAggregatedEnvelope_ = false;
    playheadMs_ = -1; update();
}

void WaveformWidget::setDarkMode(bool enabled)
{
    darkMode_ = enabled;
    update();
}

void WaveformWidget::setPlayheadPosition(qint64 milliseconds)
{
    playheadMs_ = milliseconds;
    update();
}

void WaveformWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient background(0, 0, 0, height());
    background.setColorAt(0, QColor(darkMode_ ? "#171d28" : "#fbfcff"));
    background.setColorAt(1, QColor(darkMode_ ? "#10151e" : "#eef3f9"));
    painter.fillRect(rect(), background);

    const int leftMargin = 36, rightMargin = 8, topMargin = 8, rulerHeight = 22;
    const QRect graph(leftMargin, topMargin, width() - leftMargin - rightMargin,
                      height() - topMargin - rulerHeight);
    painter.setPen(QPen(QColor(darkMode_ ? "#3b465a" : "#d4ddea"), 1));
    painter.setBrush(Qt::NoBrush); painter.drawRoundedRect(graph, 5, 5);

    if (leftMinimums_.isEmpty()) {
        painter.setPen(QColor(darkMode_ ? "#a7b3c7" : "#78869b"));
        painter.drawText(graph, Qt::AlignCenter, tr("Select an analyzed file to display its waveform"));
        return;
    }

    const int lanes = channels_ > 1 ? 2 : 1;
    const int laneHeight = graph.height() / lanes;
    painter.setPen(QPen(QColor(darkMode_ ? "#52617a" : "#b7c6da"), 1, Qt::DashLine));
    for (int grid = 1; grid < 10; ++grid) {
        const int x = graph.left() + grid * graph.width() / 10;
        painter.drawLine(x, graph.top(), x, graph.bottom());
    }
    for (int lane = 0; lane < lanes; ++lane) {
        const int center = graph.top() + laneHeight * lane + laneHeight / 2;
        const int positiveLine = center - qRound(laneHeight * .43);
        const int negativeLine = center + qRound(laneHeight * .43);
        painter.setPen(QPen(QColor(darkMode_ ? "#4b5970" : "#b5c4d8"), 1, Qt::DotLine));
        painter.drawLine(graph.left(), positiveLine, graph.right(), positiveLine);
        painter.drawLine(graph.left(), negativeLine, graph.right(), negativeLine);
        painter.drawLine(graph.left(), center - qRound(laneHeight * .215),
                         graph.right(), center - qRound(laneHeight * .215));
        painter.drawLine(graph.left(), center + qRound(laneHeight * .215),
                         graph.right(), center + qRound(laneHeight * .215));
        painter.setPen(QPen(QColor(darkMode_ ? "#536078" : "#b8c5d7"), 1));
        painter.drawLine(graph.left(), center, graph.right(), center);
        painter.setPen(QColor(darkMode_ ? "#d6dfed" : "#53627a"));
        painter.drawText(QRect(5, center - 10, 26, 20), Qt::AlignCenter,
                         lanes == 1 ? QStringLiteral("M") : lane == 0 ? QStringLiteral("L") : QStringLiteral("R"));
        painter.setPen(QColor(darkMode_ ? "#9daac0" : "#8a96a8"));
        painter.drawText(QRect(5, positiveLine - 8, 26, 16), Qt::AlignRight, QStringLiteral("+1"));
        painter.drawText(QRect(5, negativeLine - 8, 26, 16), Qt::AlignRight, QStringLiteral("−1"));
    }

    auto drawChannel = [&](const QVector<float> &minimums, const QVector<float> &maximums,
                           const QVector<float> &traceSamples,
                           int lane, const QColor &color) {
        const QRect visibleGraph = event->rect().intersected(graph);
        if (visibleGraph.isEmpty()) return;
        const int firstVisiblePixel = qMax(0, visibleGraph.left() - graph.left() - 2);
        const int lastVisiblePixel = qMin(graph.width() - 1,
                                           visibleGraph.right() - graph.left() + 2);
        const int center = graph.top() + laneHeight * lane + laneHeight / 2;
        const double amplitude = laneHeight * .43;
        QPainterPath upper, lower;
        QVector<QPointF> clippedPoints;

        QPen waveformPen(color);
        waveformPen.setWidthF(.8);
        waveformPen.setCosmetic(true);
        waveformPen.setCapStyle(Qt::RoundCap);
        waveformPen.setJoinStyle(Qt::RoundJoin);

        // Cuando hay espacio suficiente se dibuja una única línea que conecta
        // las muestras PCM reales: la misma lectura visual que un osciloscopio.
        // Una ligera compresión (1:1,2, 1:2...) todavía se entiende mejor como
        // traza continua. La envolvente sólo empieza cuando cada píxel resume
        // más de cuatro muestras, como sucede en los editores de audio.
        const double samplesPerPixel = traceSamples.size() / double(qMax(1, graph.width()));
        // Una polilinea solo representa fielmente la señal cuando cada pixel
        // contiene muy pocas muestras. Por encima de este limite unir muestras
        // produce falsos ciclos (aliasing); en ese caso usamos min/max.
        const bool oscilloscopeMode = samplesPerPixel <= 4.0;
        if (oscilloscopeMode) {
            QPainterPath trace;
            const int firstSample = qMax(0, int(qint64(firstVisiblePixel)
                * traceSamples.size() / graph.width()) - 2);
            const int lastSample = qMin(traceSamples.size() - 1,
                int(qint64(lastVisiblePixel + 1) * traceSamples.size() / graph.width()) + 2);
            for (int i = firstSample; i <= lastSample; ++i) {
                // La conversión debe hacerse antes de multiplicar. Con zoom
                // alto, int(i * width) puede desbordarse y producir un falso
                // jitter aunque las muestras PCM sean perfectamente estables.
                const qreal x = graph.left()
                    + qreal(i) * qreal(graph.width())
                        / qreal(qMax(1, traceSamples.size() - 1));
                const float value = traceSamples[i];
                const qreal y = center - value * amplitude;
                if (i == firstSample) trace.moveTo(x, y); else trace.lineTo(x, y);
                if (value >= .9999f || value <= -.9999f)
                    clippedPoints.append(QPointF(x, y));
            }
            painter.setPen(waveformPen);
            painter.drawPath(trace);
        } else {
            // Al alejarse, varias muestras ocupan un píxel. Se conservan sólo
            // los contornos máximo/mínimo para evitar aliasing y bloques sólidos.
        for (int pixel = firstVisiblePixel; pixel <= lastVisiblePixel; ++pixel) {
            int first = int(qint64(pixel) * minimums.size() / graph.width());
            int last = qMax(first + 1,
                int(qint64(pixel + 1) * minimums.size() / graph.width()));
            float low = 1.0f, high = -1.0f;
            for (int i = first; i < qMin(last, minimums.size()); ++i) {
                low = qMin(low, minimums[i]); high = qMax(high, maximums[i]);
            }
            const qreal x = graph.left() + pixel;
            const qreal highY = center - high * amplitude;
            const qreal lowY = center - low * amplitude;
            if (pixel == firstVisiblePixel) {
                upper.moveTo(x, highY); lower.moveTo(x, lowY);
            } else {
                upper.lineTo(x, highY); lower.lineTo(x, lowY);
            }
            if (high >= .9999f) clippedPoints.append(QPointF(x, highY));
            if (low <= -.9999f) clippedPoints.append(QPointF(x, lowY));
        }
        painter.setPen(waveformPen);
        QPainterPath envelope = upper;
        for (int i = lower.elementCount() - 1; i >= 0; --i) {
            const QPainterPath::Element point = lower.elementAt(i);
            envelope.lineTo(point.x, point.y);
        }
        envelope.closeSubpath();
        QColor envelopeColor = color;
        envelopeColor.setAlpha(darkMode_ ? 190 : 170);
        painter.fillPath(envelope, envelopeColor);
        QPen edgePen(color);
        edgePen.setWidthF(.55);
        edgePen.setCosmetic(true);
        QColor edgeColor = color;
        edgeColor.setAlpha(darkMode_ ? 105 : 120);
        edgePen.setColor(edgeColor);
        painter.setPen(edgePen);
        painter.drawPath(upper);
        painter.drawPath(lower);
        }

        // El clipping se señala únicamente sobre el punto que toca 0 dBFS.
        // Así se localiza el defecto sin tapar la forma de onda que hay debajo.
        QPen clippingPen(QColor("#df263b"));
        clippingPen.setWidthF(1.4);
        clippingPen.setCosmetic(true);
        clippingPen.setCapStyle(Qt::RoundCap);
        painter.setPen(clippingPen);
        if (!clippedPoints.isEmpty())
            painter.drawPoints(clippedPoints.constData(), clippedPoints.size());
    };

    drawChannel(leftMinimums_, leftMaximums_, leftTrace_, 0, QColor("#0057d9"));
    if (lanes == 2)
        drawChannel(rightMinimums_, rightMaximums_, rightTrace_, 1, QColor("#0057d9"));

    // Regla adaptativa: mantiene marcas útiles también cuando sólo se está
    // viendo un pequeño fragmento 1:1 del archivo.
    painter.setPen(QColor(darkMode_ ? "#aab6ca" : "#596980"));
    const QRect visible = event->rect().intersected(graph);
    const double millisecondsPerPixel = durationMs_ / double(qMax(1, graph.width()));
    const double desiredStep = millisecondsPerPixel * 110.0;
    const qint64 steps[] = {1, 2, 5, 10, 20, 50, 100, 200, 500,
                            1000, 2000, 5000, 10000, 30000, 60000, 300000};
    qint64 step = steps[std::size(steps) - 1];
    for (qint64 candidate : steps) {
        if (candidate >= desiredStep) { step = candidate; break; }
    }
    const qint64 visibleStartMs = qMax<qint64>(0,
        qRound64((visible.left() - graph.left()) * millisecondsPerPixel));
    const qint64 visibleEndMs = qMin(durationMs_,
        qRound64((visible.right() - graph.left()) * millisecondsPerPixel));
    for (qint64 time = (visibleStartMs / step) * step;
         time <= visibleEndMs + step; time += step) {
        const qreal x = graph.left() + qreal(time) * graph.width() / qMax<qint64>(1, durationMs_);
        painter.drawLine(QPointF(x, graph.bottom()), QPointF(x, graph.bottom() + 4));
        painter.drawText(QRectF(x - 35, graph.bottom() + 4, 70, 17), Qt::AlignCenter, timeText(time));
    }

    if (playheadMs_ >= 0 && durationMs_ > 0) {
        const qreal x = graph.left() + qreal(playheadMs_) * graph.width() / durationMs_;
        QPen playheadPen(QColor("#ff9f1c"));
        playheadPen.setWidthF(1.5);
        playheadPen.setCosmetic(true);
        painter.setPen(playheadPen);
        painter.drawLine(QPointF(x, graph.top()), QPointF(x, graph.bottom()));
        painter.setBrush(QColor("#ff9f1c"));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(QPolygonF({QPointF(x - 5, graph.top()),
                                      QPointF(x + 5, graph.top()),
                                      QPointF(x, graph.top() + 7)}));
    }
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
