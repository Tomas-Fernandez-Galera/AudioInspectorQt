#pragma once

#include <QMainWindow>
#include <QVector>

class AudioAnalyzer;
class WaveformWidget;
class QResizeEvent;
class QCloseEvent;
class QMediaPlayer;
class QAudioOutput;
class QComboBox;
class QPushButton;
class QProgressDialog;
struct DspAnalysis;
namespace Ui { class MainWindow; }

// Ventana de análisis de un único archivo. Abrir otro archivo descarta por
// completo el análisis anterior y comienza una sesión nueva.
class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void chooseFile();
    void resetAnalysis(const QString &path = {});
    bool isSupportedFile(const QString &path) const;
    void updateWaveformScale();
    void applyTheme(bool dark);
    void applyLanguage();
    void showHelp();
    void showAbout();
    void showReport();
    void showAnalysisProgress();
    void showAnalysis(const QString &path, int sampleRate, int bits,
        int channels, qint64 durationMs, int bitrateKbps, double peakDb,
        bool clipping, const QString &error,
        const QVector<float> &leftMinimums, const QVector<float> &leftMaximums,
        const QVector<float> &rightMinimums, const QVector<float> &rightMaximums,
        const QVector<float> &leftTrace, const QVector<float> &rightTrace,
        double leftRmsDb, double rightRmsDb, double leftDcOffset,
        double rightDcOffset, double silencePercent, qint64 clippedFrames,
        const DspAnalysis &dsp);

    Ui::MainWindow *ui_ = nullptr;
    AudioAnalyzer *analyzer_ = nullptr;
    WaveformWidget *waveform_ = nullptr;
    QString currentPath_;
    int currentSampleCount_ = 0;
    QString lastDirectory_;
    QMediaPlayer *player_ = nullptr;
    QAudioOutput *audioOutput_ = nullptr;
    QComboBox *languageCombo_ = nullptr;
    QPushButton *helpButton_ = nullptr;
    QPushButton *aboutButton_ = nullptr;
    QPushButton *reportButton_ = nullptr;
    QString languageCode_ = "en";
    QString lastReport_;
    QProgressDialog *analysisProgress_ = nullptr;
};
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
