#include "mainwindow.h"
#include "audioanalyzer.h"
#include "waveformwidget.h"
#include "dspanalysis.h"
#include "localization.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QCheckBox>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QCloseEvent>
#include <QDir>
#include <QDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHash>
#include <QListWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QProgressDialog>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QSlider>
#include <QSettings>
#include <QStatusBar>
#include <QStandardPaths>
#include <QTimer>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QPainter>
#include <QUrl>

#include <cmath>
#include <climits>

namespace {
// El análisis no admite cancelación parcial: cerrar el diálogo no detendría de
// forma segura QAudioDecoder ni los acumuladores DSP. Esta variante ignora todo
// intento del usuario de cerrar la ventana y solo MainWindow puede ocultarla.
class NonClosableProgressDialog final : public QProgressDialog
{
public:
    explicit NonClosableProgressDialog(QWidget *parent) : QProgressDialog(parent)
    {
        setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    }

    void reject() override { /* Escape y Alt+F4 no cancelan el análisis. */ }

protected:
    void closeEvent(QCloseEvent *event) override { event->ignore(); }
};

QString decibels(double value)
{
    return std::isfinite(value) ? QStringLiteral("%1 dBFS").arg(value, 0, 'f', 2)
                                : QStringLiteral("−∞ dBFS");
}

void addFinding(QListWidget *list, const QString &text, const QColor &color)
{
    auto *item = new QListWidgetItem(text, list);
    item->setForeground(color);
}

void configureDocumentViewer(QTextBrowser *browser)
{
    browser->setReadOnly(true);
    browser->setOpenExternalLinks(true);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    browser->setLineWrapMode(QTextEdit::WidgetWidth);
    browser->setFocusPolicy(Qt::StrongFocus);
    browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    browser->verticalScrollBar()->setSingleStep(28);
    browser->verticalScrollBar()->setPageStep(300);
}

QString clockText(qint64 milliseconds)
{
    const qint64 seconds = qMax<qint64>(0, milliseconds / 1000);
    return QStringLiteral("%1:%2").arg(seconds / 60)
        .arg(seconds % 60, 2, 10, QLatin1Char('0'));
}

bool isTemporaryDirectory(const QString &path)
{
    const QString candidate = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    const QString temporary = QDir::cleanPath(QDir::tempPath());
    return candidate.compare(temporary, Qt::CaseInsensitive) == 0
        || candidate.startsWith(temporary + QDir::separator(), Qt::CaseInsensitive);
}

QString messageFor(const QString &language, const QString &english, const QString &spanish)
{
    // Los mensajes del análisis contienen marcadores (%1, %2…). Se resuelven
    // aquí para que la lógica numérica no dependa de un idioma concreto.
    if (language == "zh") {
        static const QHash<QString, QString> chinese = {
            {"Analyzing…", "正在分析…"}, {"Analyzing %1", "正在分析 %1"},
            {"Analysis failed", "分析失败"}, {"Decoder error: %1", "解码器错误：%1"},
            {"True peak (4x estimate): %1 dBTP", "True Peak（4倍估算）：%1 dBTP"},
            {"LUFS estimate: %1 integrated · %2 max momentary · %3 max short-term", "LUFS 估算：综合 %1 · 最大瞬时 %2 · 最大短时 %3"},
            {"BPM: %1 · confidence %2% · estimated key: %3", "BPM：%1 · 置信度 %2% · 估算调性：%3"},
            {"BPM unavailable: the file is too short · estimated key: %1", "无法估算 BPM：文件过短 · 估算调性：%1"},
            {"Section dynamics: %1 dB · stereo correlation: %2 · mono change: %3 dB", "分段动态：%1 dB · 立体声相关度：%2 · 单声道变化：%3 dB"},
            {"3 s sections: quiet %1 · median %2 · loud %3 LUFS (estimate)", "3秒分段：安静 %1 · 中位 %2 · 响亮 %3 LUFS（估算）"},
            {"Spectrum: subsonic %1% · low %2% · mid %3% · high %4%", "频谱：次声 %1% · 低频 %2% · 中频 %3% · 高频 %4%"},
            {"Silence: %1 s leading · %2 s trailing", "静音：开头 %1 秒 · 结尾 %2 秒"},
            {"Clipping detected in %1 sample frames.", "在 %1 个采样帧中检测到削波。"},
            {"Low headroom: peak is above −1 dBFS.", "余量较低：峰值高于 −1 dBFS。"},
            {"Significant DC offset detected.", "检测到明显直流偏移。"},
            {"Stereo level imbalance exceeds 1.5 dB.", "立体声电平差超过 1.5 dB。"},
            {"More than 10% of the file is digital silence.", "文件中超过 10% 为数字静音。"},
            {"Sample rate is below 44.1 kHz.", "采样率低于 44.1 kHz。"},
            {"Possible mono compatibility problem.", "可能存在单声道兼容问题。"},
            {"Significant energy below 20 Hz detected.", "检测到明显的 20 Hz 以下能量。"},
            {"Long leading or trailing silence detected.", "检测到较长的首部或尾部静音。"},
            {"BPM estimate has low confidence; treat it as uncertain.", "BPM 估算置信度较低，应视为不确定。"},
            {"No obvious technical problems detected.", "未检测到明显技术问题。"},
            {"Analysis complete · %1 finding(s)", "分析完成 · %1 个提示"},
            {"Analysis complete", "分析完成"}
        };
        return chinese.value(english, english);
    }
    return language == "es" ? spanish : english;
}

QIcon flagIcon(const QString &code)
{
    QPixmap image(30, 20);
    image.fill(Qt::transparent);
    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRect r(1, 2, 28, 16);
    p.setPen(QPen(QColor("#9aa3af"), 1));
    p.setBrush(Qt::white); p.drawRect(r);
    p.setPen(Qt::NoPen);
    if (code == "en") {
        p.fillRect(r, QColor("#173a78"));
        p.setPen(QPen(Qt::white, 4)); p.drawLine(r.topLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomLeft());
        p.setPen(QPen(QColor("#d42336"), 2)); p.drawLine(r.topLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomLeft());
        p.fillRect(QRect(r.left(), r.center().y() - 2, r.width(), 5), Qt::white);
        p.fillRect(QRect(r.center().x() - 2, r.top(), 5, r.height()), Qt::white);
        p.fillRect(QRect(r.left(), r.center().y() - 1, r.width(), 3), QColor("#d42336"));
        p.fillRect(QRect(r.center().x() - 1, r.top(), 3, r.height()), QColor("#d42336"));
    } else if (code == "es") {
        p.fillRect(r, QColor("#aa151b"));
        p.fillRect(QRect(r.left(), r.top() + 4, r.width(), 8), QColor("#f1bf00"));
    } else if (code == "ca") {
        p.fillRect(r, QColor("#f6cf16"));
        for (int y = r.top() + 2; y < r.bottom(); y += 4)
            p.fillRect(QRect(r.left(), y, r.width(), 2), QColor("#d5232f"));
    } else if (code == "gl") {
        p.fillRect(r, Qt::white);
        p.setPen(QPen(QColor("#55a9df"), 4));
        p.drawLine(r.left(), r.bottom(), r.right(), r.top());
    } else if (code == "eu") {
        p.fillRect(r, QColor("#d71920"));
        p.setPen(QPen(QColor("#138447"), 4)); p.drawLine(r.topLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomLeft());
        p.fillRect(QRect(r.left(), r.center().y() - 1, r.width(), 3), Qt::white);
        p.fillRect(QRect(r.center().x() - 1, r.top(), 3, r.height()), Qt::white);
    } else if (code == "fr" || code == "it") {
        const QColor first = code == "fr" ? QColor("#173f8a") : QColor("#16834a");
        const QColor last = code == "fr" ? QColor("#df263b") : QColor("#d52b36");
        p.fillRect(QRect(r.left(), r.top(), r.width() / 3, r.height()), first);
        p.fillRect(QRect(r.left() + r.width() / 3, r.top(), r.width() / 3, r.height()), Qt::white);
        p.fillRect(QRect(r.left() + 2 * r.width() / 3, r.top(), r.width() - 2 * r.width() / 3, r.height()), last);
    } else if (code == "de") {
        p.fillRect(QRect(r.left(), r.top(), r.width(), 5), QColor("#171717"));
        p.fillRect(QRect(r.left(), r.top() + 5, r.width(), 6), QColor("#d5232f"));
        p.fillRect(QRect(r.left(), r.top() + 11, r.width(), 5), QColor("#f2c500"));
    } else if (code == "pt") {
        p.fillRect(r, QColor("#d62436"));
        p.fillRect(QRect(r.left(), r.top(), 11, r.height()), QColor("#177245"));
        p.setBrush(QColor("#f4c542")); p.drawEllipse(QPointF(r.left() + 11, r.center().y()), 3, 3);
    } else if (code == "ja") {
        p.fillRect(r, Qt::white); p.setBrush(QColor("#bc002d"));
        p.drawEllipse(QPointF(r.center()), 4, 4);
    } else if (code == "ko") {
        p.fillRect(r, Qt::white); p.setBrush(QColor("#d82b3c"));
        p.drawPie(QRect(r.center().x() - 4, r.center().y() - 4, 8, 8), 0, 180 * 16);
        p.setBrush(QColor("#1754a5")); p.drawPie(QRect(r.center().x() - 4, r.center().y() - 4, 8, 8), 180 * 16, 180 * 16);
    } else if (code == "zh") {
        p.fillRect(r, QColor("#de2910")); p.setBrush(QColor("#ffde00"));
        p.drawEllipse(QPointF(r.left() + 6, r.top() + 5), 2, 2);
    }
    p.setPen(QPen(QColor("#7d8795"), 1)); p.setBrush(Qt::NoBrush); p.drawRect(r);
    return QIcon(image);
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow),
      analyzer_(new AudioAnalyzer(this))
{
    // La ventana coordina componentes, pero no calcula audio: AudioAnalyzer
    // produce medidas y WaveformWidget se limita a representarlas.
    ui_->setupUi(this);
    waveform_ = ui_->waveformWidget;
    player_ = new QMediaPlayer(this);
    audioOutput_ = new QAudioOutput(this);
    player_->setAudioOutput(audioOutput_);
    audioOutput_->setMuted(false);
    // Progreso indeterminado: QAudioDecoder no informa un porcentaje fiable,
    // de modo que una animación evita prometer tiempos o avances inventados.
    analysisProgress_ = new NonClosableProgressDialog(this);
    // Se muestra manualmente al recibir una ruta. Un mínimo muy alto desactiva
    // el temporizador interno de QProgressDialog, que de otro modo podría abrir
    // la ventana durante el arranque aunque aún no exista ningún trabajo.
    analysisProgress_->setMinimumDuration(INT_MAX);
    analysisProgress_->setRange(0, 0);
    analysisProgress_->setCancelButton(nullptr);
    analysisProgress_->setAutoClose(false);
    analysisProgress_->setAutoReset(false);
    analysisProgress_->setWindowModality(Qt::WindowModal);
    analysisProgress_->setWindowTitle(QStringLiteral("AudioInspector Qt"));
    analysisProgress_->reset();
    analysisProgress_->hide();
    // Una única barra compacta: el archivo y el estado pasan a la cabecera.
    ui_->subtitleLabel->hide();
    ui_->mainLayout->removeWidget(ui_->fileCard);
    auto *compactFileLayout = new QVBoxLayout;
    compactFileLayout->setContentsMargins(8, 0, 8, 0);
    compactFileLayout->setSpacing(0);
    compactFileLayout->addWidget(ui_->fileNameLabel);
    compactFileLayout->addWidget(ui_->filePathLabel);
    ui_->headerLayout->insertLayout(1, compactFileLayout, 1);
    ui_->headerLayout->insertWidget(2, ui_->analysisStatusLabel);
    languageCombo_ = new QComboBox(this);
    languageCombo_->addItem(flagIcon("en"), QStringLiteral("English"), QStringLiteral("en"));
    languageCombo_->addItem(flagIcon("es"), QStringLiteral("Español"), QStringLiteral("es"));
    languageCombo_->addItem(flagIcon("ca"), QStringLiteral("Català"), QStringLiteral("ca"));
    languageCombo_->addItem(flagIcon("gl"), QStringLiteral("Galego"), QStringLiteral("gl"));
    languageCombo_->addItem(flagIcon("eu"), QStringLiteral("Euskara"), QStringLiteral("eu"));
    languageCombo_->addItem(flagIcon("fr"), QStringLiteral("Français"), QStringLiteral("fr"));
    languageCombo_->addItem(flagIcon("de"), QStringLiteral("Deutsch"), QStringLiteral("de"));
    languageCombo_->addItem(flagIcon("it"), QStringLiteral("Italiano"), QStringLiteral("it"));
    languageCombo_->addItem(flagIcon("pt"), QStringLiteral("Português"), QStringLiteral("pt"));
    languageCombo_->addItem(flagIcon("ja"), QStringLiteral("日本語"), QStringLiteral("ja"));
    languageCombo_->addItem(flagIcon("ko"), QStringLiteral("한국어"), QStringLiteral("ko"));
    languageCombo_->addItem(flagIcon("zh"), QStringLiteral("中文"), QStringLiteral("zh"));
    languageCombo_->setIconSize(QSize(30, 20));
    languageCombo_->setMinimumWidth(142);
    helpButton_ = new QPushButton(this);
    aboutButton_ = new QPushButton(this);
    reportButton_ = new QPushButton(this);
    reportButton_->setEnabled(false);
    ui_->issuesLayout->addWidget(reportButton_);
    ui_->headerLayout->insertWidget(3, languageCombo_);
    ui_->headerLayout->insertWidget(4, helpButton_);
    ui_->headerLayout->insertWidget(5, aboutButton_);
    ui_->fileCard->hide();
    ui_->mainLayout->setStretchFactor(ui_->waveformScrollArea, 5);
    ui_->mainLayout->setStretchFactor(ui_->resultsSplitter, 1);
    connect(ui_->openButton, &QPushButton::clicked, this, &MainWindow::chooseFile);
    connect(helpButton_, &QPushButton::clicked, this, &MainWindow::showHelp);
    connect(aboutButton_, &QPushButton::clicked, this, &MainWindow::showAbout);
    connect(reportButton_, &QPushButton::clicked, this, &MainWindow::showReport);
    connect(languageCombo_, &QComboBox::currentIndexChanged, this, [this] {
        // Los hallazgos y el informe ya contienen texto. Reanalizar al cambiar
        // idioma evita conservar resultados escritos en la selección anterior.
        const QString pathToReanalyze = currentPath_;
        languageCode_ = languageCombo_->currentData().toString();
        QSettings().setValue(QStringLiteral("interface/language"), languageCode_);
        applyLanguage();
        if (!pathToReanalyze.isEmpty() && QFileInfo::exists(pathToReanalyze)) {
            resetAnalysis(pathToReanalyze);
            analyzer_->analyzeFile(pathToReanalyze);
            showAnalysisProgress();
        }
    });
    connect(analyzer_, &AudioAnalyzer::analysisFinished, this, &MainWindow::showAnalysis);
    connect(ui_->zoomSlider, &QSlider::valueChanged, this, [this] { updateWaveformScale(); });
    connect(waveform_, &WaveformWidget::scaleWheelRequested, this,
            [this](int steps, double filePosition) {
        // Mantiene bajo el puntero la misma zona aproximada al usar la rueda.
        const int oldMaximum = ui_->waveformScrollArea->horizontalScrollBar()->maximum();
        const int change = -steps * 35;
        ui_->zoomSlider->setValue(qBound(0, ui_->zoomSlider->value() + change, 1000));
        QScrollBar *bar = ui_->waveformScrollArea->horizontalScrollBar();
        if (bar->maximum() > 0) {
            const int target = qRound(filePosition * (bar->maximum() + bar->pageStep())
                                      - bar->pageStep() / 2.0);
            bar->setValue(qBound(0, target, bar->maximum()));
        } else if (oldMaximum > 0) {
            bar->setValue(0);
        }
    });
    connect(ui_->actualSamplesButton, &QPushButton::clicked, this, [this] {
        ui_->zoomSlider->setValue(0);
        ui_->waveformScrollArea->horizontalScrollBar()->setValue(0);
    });
    connect(ui_->fitWaveformButton, &QPushButton::clicked, this, [this] {
        ui_->zoomSlider->setValue(1000);
        ui_->waveformScrollArea->horizontalScrollBar()->setValue(0);
    });
    connect(ui_->playPauseButton, &QPushButton::clicked, this, [this] {
        if (player_->playbackState() == QMediaPlayer::PlayingState) player_->pause();
        else player_->play();
    });
    connect(ui_->stopButton, &QPushButton::clicked, this, [this] {
        player_->stop();
        player_->setPosition(0);
    });
    connect(ui_->volumeSlider, &QSlider::valueChanged, this,
            [this](int value) { audioOutput_->setVolume(value / 100.0f); });
    connect(ui_->playbackSlider, &QSlider::sliderMoved, player_, &QMediaPlayer::setPosition);
    connect(player_, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui_->playbackSlider->setMaximum(int(qMin<qint64>(duration, INT_MAX)));
        ui_->playbackTimeLabel->setText(QStringLiteral("%1 / %2")
            .arg(clockText(player_->position()), clockText(duration)));
    });
    connect(player_, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        // Sincroniza control, contador, cursor naranja y desplazamiento automático.
        if (!ui_->playbackSlider->isSliderDown()) ui_->playbackSlider->setValue(int(position));
        ui_->playbackTimeLabel->setText(QStringLiteral("%1 / %2")
            .arg(clockText(position), clockText(player_->duration())));
        waveform_->setPlayheadPosition(position);
        QScrollBar *bar = ui_->waveformScrollArea->horizontalScrollBar();
        if (player_->playbackState() == QMediaPlayer::PlayingState && bar->maximum() > 0
            && player_->duration() > 0) {
            const int x = qRound(position / double(player_->duration())
                                 * (bar->maximum() + bar->pageStep()));
            const int margin = bar->pageStep() / 10;
            if (x < bar->value() + margin || x > bar->value() + bar->pageStep() - margin)
                bar->setValue(qBound(0, x - bar->pageStep() / 2, bar->maximum()));
        }
    });
    connect(player_, &QMediaPlayer::playbackStateChanged, this,
            [this](QMediaPlayer::PlaybackState state) {
        ui_->playPauseButton->setText(Localization::text(
            state == QMediaPlayer::PlayingState ? "pause" : "play", languageCode_));
    });
    connect(player_, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString &description) {
        if (error == QMediaPlayer::NoError) return;
        statusBar()->showMessage(messageFor(languageCode_,
            QStringLiteral("Playback error: %1"),
            QStringLiteral("Error de reproducción: %1")).arg(description), 8000);
    });
    QTimer::singleShot(0, this, &MainWindow::updateWaveformScale);

    QSettings settings;
    languageCode_ = settings.value(QStringLiteral("interface/language"), QStringLiteral("en")).toString();
    const int languageIndex = languageCombo_->findData(languageCode_);
    languageCombo_->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);
    lastDirectory_ = settings.value(QStringLiteral("files/lastDirectory")).toString();
    if (isTemporaryDirectory(lastDirectory_)) lastDirectory_.clear();
    ui_->zoomSlider->setValue(settings.value(QStringLiteral("view/timeScale"), 1000).toInt());
    ui_->volumeSlider->setValue(settings.value(QStringLiteral("playback/volume"), 70).toInt());
    ui_->darkThemeCheck->setChecked(settings.value(QStringLiteral("view/darkTheme"), false).toBool());
    ui_->resultsSplitter->restoreState(settings.value(QStringLiteral("view/resultsSplitter")).toByteArray());
    audioOutput_->setVolume(ui_->volumeSlider->value() / 100.0f);
    connect(ui_->darkThemeCheck, &QCheckBox::toggled, this, &MainWindow::applyTheme);
    applyTheme(ui_->darkThemeCheck->isChecked());
    applyLanguage();
    resetAnalysis();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // El viewport obtiene su tamaño definitivo después del layout. Se agenda
    // el ajuste para que maximizar/restaurar también rellene todo el panel.
    QTimer::singleShot(0, this, &MainWindow::updateWaveformScale);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Solo se guardan preferencias de interfaz. No se persisten medidas ni rutas
    // temporales de archivos, evitando abrir sesiones caducadas al arrancar.
    QSettings settings;
    settings.setValue(QStringLiteral("files/lastDirectory"), lastDirectory_);
    settings.setValue(QStringLiteral("view/timeScale"), ui_->zoomSlider->value());
    settings.setValue(QStringLiteral("view/darkTheme"), ui_->darkThemeCheck->isChecked());
    settings.setValue(QStringLiteral("view/resultsSplitter"), ui_->resultsSplitter->saveState());
    settings.setValue(QStringLiteral("playback/volume"), ui_->volumeSlider->value());
    QMainWindow::closeEvent(event);
}

void MainWindow::showHelp()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("AudioInspector Qt — %1")
        .arg(Localization::text("help", languageCode_)));
    dialog.resize(760, 680);
    auto *layout = new QVBoxLayout(&dialog);
    auto *browser = new QTextBrowser(&dialog);
    configureDocumentViewer(browser);
    browser->setHtml(Localization::helpHtml(languageCode_));
    browser->moveCursor(QTextCursor::Start);
    browser->setFocus();
    auto *closeButton = new QPushButton(Localization::text("close", languageCode_), &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(browser, 1);
    layout->addWidget(closeButton, 0, Qt::AlignRight);
    dialog.exec();
}

void MainWindow::showAbout()
{
    static const QHash<QString, QString> captions = {
        {"en", "About"}, {"es", "Acerca de"}, {"ca", "Quant a"},
        {"gl", "Acerca de"}, {"eu", "Honi buruz"}, {"fr", "À propos"},
        {"de", "Über"}, {"it", "Informazioni"}, {"pt", "Sobre"},
        {"ja", "情報"}, {"ko", "정보"}, {"zh", "关于"}
    };
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("AudioInspector Qt — %1")
        .arg(captions.value(languageCode_, QStringLiteral("About"))));
    dialog.resize(650, 480);
    auto *layout = new QVBoxLayout(&dialog);
    auto *browser = new QTextBrowser(&dialog);
    configureDocumentViewer(browser);
    browser->setHtml(QStringLiteral("<h1>AudioInspector Qt</h1><p>Version %1</p>")
        .arg(QStringLiteral("0.1.0"))
        + Localization::legalHtml(languageCode_)
        + Localization::principlesHtml(languageCode_));
    browser->moveCursor(QTextCursor::Start);
    browser->setFocus();
    auto *closeButton = new QPushButton(Localization::text("close", languageCode_), &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(browser, 1);
    layout->addWidget(closeButton, 0, Qt::AlignRight);
    dialog.exec();
}

void MainWindow::showReport()
{
    // El informe es texto plano a propósito: puede copiarse a foros, incidencias,
    // correo o documentación sin perder datos dentro de un formato propietario.
    if (lastReport_.isEmpty()) return;
    QDialog dialog(this);
    dialog.setWindowTitle(languageCode_ == "es" ? QStringLiteral("Informe técnico")
        : languageCode_ == "zh" ? QStringLiteral("技术报告") : QStringLiteral("Technical report"));
    dialog.resize(780, 680);
    auto *layout = new QVBoxLayout(&dialog);
    auto *editor = new QPlainTextEdit(&dialog);
    editor->setReadOnly(true);
    editor->setPlainText(lastReport_);
    editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    auto *buttons = new QHBoxLayout;
    auto *copyButton = new QPushButton(languageCode_ == "es" ? QStringLiteral("Copiar informe")
        : languageCode_ == "zh" ? QStringLiteral("复制报告") : QStringLiteral("Copy report"), &dialog);
    auto *closeButton = new QPushButton(Localization::text("close", languageCode_), &dialog);
    connect(copyButton, &QPushButton::clicked, this, [this, copyButton] {
        QApplication::clipboard()->setText(lastReport_);
        copyButton->setText(languageCode_ == "es" ? QStringLiteral("Copiado")
            : languageCode_ == "zh" ? QStringLiteral("已复制") : QStringLiteral("Copied"));
    });
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    buttons->addWidget(copyButton);
    buttons->addStretch();
    buttons->addWidget(closeButton);
    layout->addWidget(editor, 1);
    layout->addLayout(buttons);
    dialog.exec();
}

void MainWindow::showAnalysisProgress()
{
    // Se llama únicamente DESPUÉS de AudioAnalyzer::analyzeFile. Mantener esta
    // secuencia impide que un diálogo modal bloquee el inicio del decoder.
    if (currentPath_.isEmpty()) return;
    analysisProgress_->setLabelText(languageCode_ == "es" ? QStringLiteral("Cargando y analizando el archivo…")
        : languageCode_ == "zh" ? QStringLiteral("正在加载并分析文件…")
                                : QStringLiteral("Loading and analyzing file…"));
    analysisProgress_->show();
    analysisProgress_->raise();
    analysisProgress_->activateWindow();
}

void MainWindow::applyLanguage()
{
    // Traduce widgets estáticos. Los resultados dinámicos se reconstruyen desde
    // el callback del selector porque pueden contener números y conclusiones.
    using Localization::text;
    ui_->openButton->setText(text("open", languageCode_));
    helpButton_->setText(text("help", languageCode_));
    static const QHash<QString, QString> aboutCaptions = {
        {"en", "About"}, {"es", "Acerca de"}, {"ca", "Quant a"},
        {"gl", "Acerca de"}, {"eu", "Honi buruz"}, {"fr", "À propos"},
        {"de", "Über"}, {"it", "Informazioni"}, {"pt", "Sobre"},
        {"ja", "情報"}, {"ko", "정보"}, {"zh", "关于"}
    };
    aboutButton_->setText(aboutCaptions.value(languageCode_, QStringLiteral("About")));
    reportButton_->setText(languageCode_ == "es" ? QStringLiteral("Ver informe técnico…")
        : languageCode_ == "zh" ? QStringLiteral("查看技术报告…") : QStringLiteral("View technical report…"));
    analysisProgress_->setLabelText(languageCode_ == "es" ? QStringLiteral("Cargando y analizando el archivo…")
        : languageCode_ == "zh" ? QStringLiteral("正在加载并分析文件…")
                                : QStringLiteral("Loading and analyzing file…"));
    ui_->zoomLabel->setText(text("timeScale", languageCode_));
    ui_->actualSamplesButton->setText(text("samples", languageCode_));
    ui_->fitWaveformButton->setText(text("fullFile", languageCode_));
    ui_->darkThemeCheck->setText(text("dark", languageCode_));
    ui_->playPauseButton->setText(text(player_->playbackState() == QMediaPlayer::PlayingState
        ? "pause" : "play", languageCode_));
    ui_->stopButton->setText(text("stop", languageCode_));
    ui_->volumeLabel->setText(text("volume", languageCode_));
    ui_->technicalGroup->setTitle(text("technical", languageCode_));
    ui_->issuesGroup->setTitle(text("advanced", languageCode_));
    ui_->formatTitle->setText(text("format", languageCode_));
    ui_->sampleRateTitle->setText(text("sampleRate", languageCode_));
    ui_->depthTitle->setText(text("depth", languageCode_));
    ui_->channelsTitle->setText(text("channels", languageCode_));
    ui_->durationTitle->setText(text("duration", languageCode_));
    ui_->peakTitle->setText(text("peak", languageCode_));
    ui_->rmsLeftTitle->setText(text("rmsLeft", languageCode_));
    ui_->rmsRightTitle->setText(text("rmsRight", languageCode_));
    ui_->dcTitle->setText(text("dc", languageCode_));
    ui_->silenceTitle->setText(text("silence", languageCode_));
    ui_->crestTitle->setText(text("crest", languageCode_));
    ui_->clippedTitle->setText(text("clipped", languageCode_));
    if (currentPath_.isEmpty()) {
        ui_->fileNameLabel->setText(text("noFile", languageCode_));
        ui_->filePathLabel->setText(text("choose", languageCode_));
        ui_->analysisStatusLabel->setText(text("ready", languageCode_));
        ui_->spectrogramLabel->setText(text("spectrogram", languageCode_));
        ui_->spectrumLabel->setText(text("spectrum", languageCode_));
    }
}

void MainWindow::applyTheme(bool dark)
{
    waveform_->setDarkMode(dark);
    if (dark) {
        setStyleSheet(QStringLiteral(R"(
            QMainWindow, QWidget { background: #111722; color: #d9e2f0; font-family: "Segoe UI"; font-size: 10pt; }
            QFrame#headerCard, QGroupBox { background: #1a2230; border: 1px solid #354156; border-radius: 9px; }
            QFrame#headerCard { padding: 2px; }
            QLabel#titleLabel { color: #f2f6fc; font-size: 15pt; font-weight: 700; }
            QLabel#filePathLabel { color: #98a7bc; }
            QLabel#fileNameLabel { color: #f2f6fc; font-size: 11pt; font-weight: 650; }
            QLabel#analysisStatusLabel { color: #59a0ff; font-weight: 600; }
            QPushButton { background: #293448; color: #e5ecf7; border: 0; border-radius: 7px; padding: 8px 14px; }
            QPushButton:hover { background: #34435b; }
            QPushButton#openButton { background: #0869dd; color: white; font-weight: 650; }
            QSlider::groove:horizontal { height: 6px; background: #465269; border-radius: 3px; }
            QSlider::sub-page:horizontal { background: #1683ff; border-radius: 3px; }
            QSlider::handle:horizontal { background: #f4f7fc; border: 2px solid #1683ff; width: 16px; margin: -6px 0; border-radius: 9px; }
            QSlider::handle:horizontal:hover { background: white; border-color: #69b0ff; }
            QGroupBox { margin-top: 10px; padding-top: 12px; font-weight: 650; }
            QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
            QListWidget { border: 0; background: #1a2230; }
            QScrollArea, QScrollBar { background: #151c28; }
            QStatusBar { background: #1a2230; color: #98a7bc; }
        )"));
    } else {
        setStyleSheet(QStringLiteral(R"(
            QMainWindow { background: #f2f5f9; }
            QWidget { color: #233047; font-family: "Segoe UI"; font-size: 10pt; }
            QFrame#headerCard, QGroupBox { background: white; border: 1px solid #dbe3ee; border-radius: 9px; }
            QFrame#headerCard { padding: 2px; }
            QLabel#titleLabel { color: #10264d; font-size: 15pt; font-weight: 700; }
            QLabel#filePathLabel { color: #66758d; }
            QLabel#fileNameLabel { color: #10264d; font-size: 11pt; font-weight: 650; }
            QLabel#analysisStatusLabel { color: #0057d9; font-weight: 600; }
            QPushButton { background: #e9eef7; border: 0; border-radius: 7px; padding: 8px 14px; }
            QPushButton:hover { background: #dce5f3; }
            QPushButton#openButton { background: #0057d9; color: white; font-weight: 650; }
            QSlider::groove:horizontal { height: 6px; background: #c7d2e2; border-radius: 3px; }
            QSlider::sub-page:horizontal { background: #0057d9; border-radius: 3px; }
            QSlider::handle:horizontal { background: white; border: 2px solid #0057d9; width: 16px; margin: -6px 0; border-radius: 9px; }
            QGroupBox { margin-top: 10px; padding-top: 12px; font-weight: 650; }
            QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
            QListWidget { border: 0; background: transparent; }
            QStatusBar { background: white; color: #66758d; }
        )"));
    }
    // Si se cambia el tema después de analizar, actualiza también los textos
    // informativos que ya estaban en la lista.
    for (int i = 0; i < ui_->issuesList->count(); ++i) {
        QListWidgetItem *item = ui_->issuesList->item(i);
        const QColor color = item->foreground().color();
        if (color == QColor("#0057d9") || color == QColor("#66b2ff"))
            item->setForeground(dark ? QColor("#66b2ff") : QColor("#0057d9"));
    }
}

void MainWindow::chooseFile()
{
    QString initialDirectory = lastDirectory_;
    if (!QDir(initialDirectory).exists() || isTemporaryDirectory(initialDirectory)) {
        const QStringList musicLocations = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        initialDirectory = musicLocations.isEmpty() ? QDir::homePath() : musicLocations.first();
    }
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Open audio file"), initialDirectory,
        QStringLiteral("Audio files (*.wav *.wave *.flac *.mp3 *.ogg *.oga *.opus *.m4a *.aac *.wma *.aif *.aiff);;All files (*.*)"));
    if (path.isEmpty()) return;
    if (!isSupportedFile(path)) {
        statusBar()->showMessage(QStringLiteral("Unsupported audio file"), 4000);
        return;
    }
    const QString selectedDirectory = QFileInfo(path).absolutePath();
    if (!isTemporaryDirectory(selectedDirectory)) lastDirectory_ = selectedDirectory;
    resetAnalysis(path);
    analyzer_->analyzeFile(currentPath_);
    showAnalysisProgress();
}

bool MainWindow::isSupportedFile(const QString &path) const
{
    static const QSet<QString> extensions = {"wav", "wave", "flac", "mp3", "ogg", "oga",
        "opus", "m4a", "aac", "wma", "aif", "aiff"};
    const QFileInfo info(path);
    return info.isFile() && extensions.contains(info.suffix().toLower());
}

void MainWindow::resetAnalysis(const QString &path)
{
    // Un nuevo archivo invalida simultáneamente reproducción, gráficas, métricas
    // e informe; ningún dato de la sesión anterior debe sobrevivir.
    player_->stop();
    player_->setSource(QUrl());
    ui_->playbackSlider->setRange(0, 0);
    ui_->playbackTimeLabel->setText(QStringLiteral("0:00 / 0:00"));
    currentPath_ = path.isEmpty() ? QString{} : QFileInfo(path).absoluteFilePath();
    // Toda ruta válida vuelve a cargarse también en el reproductor. Esto es
    // esencial cuando resetAnalysis se ejecuta por un cambio de idioma.
    if (!currentPath_.isEmpty())
        player_->setSource(QUrl::fromLocalFile(currentPath_));
    // Mostrar el progreso aquí sería demasiado pronto: resetAnalysis también
    // se usa al arrancar y antes de iniciar el decoder. chooseFile o el cambio
    // de idioma lo muestran explícitamente después de analyzeFile.
    analysisProgress_->hide();
    currentSampleCount_ = 0;
    waveform_->clearWaveform();
    ui_->waveformScrollArea->horizontalScrollBar()->setValue(0);
    ui_->issuesList->clear();
    lastReport_.clear();
    reportButton_->setEnabled(false);
    ui_->spectrogramLabel->clear();
    ui_->spectrogramLabel->setText(Localization::text("spectrogram", languageCode_));
    ui_->spectrumLabel->clear();
    ui_->spectrumLabel->setText(Localization::text("spectrum", languageCode_));
    const QList<QLabel *> values = {ui_->formatValue, ui_->sampleRateValue, ui_->depthValue,
        ui_->channelsValue, ui_->durationValue, ui_->peakValue, ui_->rmsLeftValue,
        ui_->rmsRightValue, ui_->dcValue, ui_->silenceValue, ui_->crestValue,
        ui_->clippedValue};
    for (QLabel *label : values) label->setText(QStringLiteral("—"));
    if (path.isEmpty()) {
        ui_->fileNameLabel->setText(Localization::text("noFile", languageCode_));
        ui_->filePathLabel->setText(Localization::text("choose", languageCode_));
        ui_->analysisStatusLabel->setText(Localization::text("ready", languageCode_));
    } else {
        const QFileInfo info(path);
        ui_->fileNameLabel->setText(info.fileName());
        ui_->filePathLabel->setText(info.absoluteFilePath());
        ui_->formatValue->setText(info.suffix().toUpper());
        ui_->analysisStatusLabel->setText(messageFor(languageCode_, "Analyzing…", "Analizando…"));
        statusBar()->showMessage(messageFor(languageCode_, "Analyzing %1", "Analizando %1").arg(info.fileName()));
    }
    updateWaveformScale();
}

void MainWindow::showAnalysis(const QString &path, int sampleRate, int bits,
    int channels, qint64 durationMs, int bitrateKbps, double peakDb,
    bool clipping, const QString &error,
    const QVector<float> &leftMinimums, const QVector<float> &leftMaximums,
    const QVector<float> &rightMinimums, const QVector<float> &rightMaximums,
    const QVector<float> &leftTrace, const QVector<float> &rightTrace,
    double leftRmsDb, double rightRmsDb, double leftDcOffset,
    double rightDcOffset, double silencePercent, qint64 clippedFrames,
    const DspAnalysis &dsp)
{
    // QAudioDecoder puede terminar después de que el usuario abra otro archivo.
    // Esta comprobación descarta resultados asíncronos que ya no corresponden.
    if (QFileInfo(path).absoluteFilePath() != currentPath_) return;
    analysisProgress_->hide();
    if (!error.isEmpty()) {
        ui_->analysisStatusLabel->setText(messageFor(languageCode_, "Analysis failed", "Error de análisis"));
        addFinding(ui_->issuesList, messageFor(languageCode_, "Decoder error: %1", "Error del decodificador: %1").arg(error), QColor("#c62828"));
        statusBar()->showMessage(messageFor(languageCode_, "Analysis failed", "Error de análisis"));
        return;
    }
    currentSampleCount_ = leftTrace.size();
    waveform_->setWaveform(leftMinimums, leftMaximums, rightMinimums, rightMaximums,
                           leftTrace, rightTrace,
                           channels, durationMs);
    ui_->sampleRateValue->setText(QStringLiteral("%1 Hz").arg(sampleRate));
    ui_->depthValue->setText(QStringLiteral("%1-bit decoded PCM").arg(bits));
    ui_->channelsValue->setText(channels == 1 ? QStringLiteral("1 · Mono")
                                              : QStringLiteral("%1 · Stereo").arg(channels));
    ui_->durationValue->setText(QStringLiteral("%1:%2 · %3 kbps")
        .arg(durationMs / 60000).arg((durationMs / 1000) % 60, 2, 10, QLatin1Char('0')).arg(bitrateKbps));
    ui_->peakValue->setText(decibels(peakDb));
    ui_->rmsLeftValue->setText(decibels(leftRmsDb));
    ui_->rmsRightValue->setText(channels > 1 ? decibels(rightRmsDb) : QStringLiteral("Mono"));
    ui_->dcValue->setText(QStringLiteral("%1% / %2%")
        .arg(leftDcOffset * 100.0, 0, 'f', 3).arg(rightDcOffset * 100.0, 0, 'f', 3));
    ui_->silenceValue->setText(QStringLiteral("%1%").arg(silencePercent, 0, 'f', 2));
    const double referenceRms = channels > 1 ? (leftRmsDb + rightRmsDb) / 2.0 : leftRmsDb;
    ui_->crestValue->setText(std::isfinite(peakDb) && std::isfinite(referenceRms)
        ? QStringLiteral("%1 dB").arg(peakDb - referenceRms, 0, 'f', 2)
        : QStringLiteral("—"));
    ui_->clippedValue->setText(QString::number(clippedFrames));

    // Azul = información medida/estimada; naranja = precaución; rojo = defecto
    // claro; verde = no se han encontrado problemas según los umbrales actuales.
    int warnings = 0;
    const QColor informationColor = ui_->darkThemeCheck->isChecked()
        ? QColor("#66b2ff") : QColor("#0057d9");
    QString displayedKey = dsp.musicalKey;
    if (languageCode_ == "es") {
        displayedKey.replace(QStringLiteral(" major"), QStringLiteral(" mayor"));
        displayedKey.replace(QStringLiteral(" minor"), QStringLiteral(" menor"));
    }
    if (!dsp.spectrogram.isNull())
        ui_->spectrogramLabel->setPixmap(QPixmap::fromImage(dsp.spectrogram));
    if (!dsp.spectrumPlot.isNull())
        ui_->spectrumLabel->setPixmap(QPixmap::fromImage(dsp.spectrumPlot));
    addFinding(ui_->issuesList, messageFor(languageCode_, "True peak (4x estimate): %1 dBTP", "True peak (estimación 4x): %1 dBTP")
        .arg(dsp.truePeakDbtp, 0, 'f', 2), informationColor);
    addFinding(ui_->issuesList, messageFor(languageCode_, "LUFS estimate: %1 integrated · %2 max momentary · %3 max short-term", "Estimación LUFS: %1 integrada · %2 momentánea máxima · %3 short-term máxima")
        .arg(dsp.integratedLufs, 0, 'f', 1).arg(dsp.momentaryLufs, 0, 'f', 1)
        .arg(dsp.shortTermLufs, 0, 'f', 1), informationColor);
    if (dsp.bpm > 0.0)
        addFinding(ui_->issuesList, messageFor(languageCode_, "BPM: %1 · confidence %2% · estimated key: %3", "BPM: %1 · confianza %2% · tonalidad estimada: %3")
            .arg(dsp.bpm, 0, 'f', 1).arg(dsp.bpmConfidence, 0, 'f', 0).arg(displayedKey), informationColor);
    else
        addFinding(ui_->issuesList, messageFor(languageCode_, "BPM unavailable: the file is too short · estimated key: %1", "BPM no disponible: el archivo es demasiado corto · tonalidad estimada: %1")
            .arg(displayedKey), informationColor);
    addFinding(ui_->issuesList, messageFor(languageCode_, "Section dynamics: %1 dB · stereo correlation: %2 · mono change: %3 dB", "Dinámica por secciones: %1 dB · correlación estéreo: %2 · cambio mono: %3 dB")
        .arg(dsp.dynamicRangeDb, 0, 'f', 1).arg(dsp.correlation, 0, 'f', 3)
        .arg(dsp.monoCompatibilityDb, 0, 'f', 2), informationColor);
    addFinding(ui_->issuesList, messageFor(languageCode_, "3 s sections: quiet %1 · median %2 · loud %3 LUFS (estimate)", "Secciones de 3 s: suave %1 · mediana %2 · fuerte %3 LUFS (estimación)")
        .arg(dsp.quietSectionLufs, 0, 'f', 1).arg(dsp.medianSectionLufs, 0, 'f', 1)
        .arg(dsp.loudSectionLufs, 0, 'f', 1), informationColor);
    addFinding(ui_->issuesList, messageFor(languageCode_, "Spectrum: subsonic %1% · low %2% · mid %3% · high %4%", "Espectro: subsónico %1% · graves %2% · medios %3% · agudos %4%")
        .arg(dsp.subsonicPercent, 0, 'f', 2).arg(dsp.lowPercent, 0, 'f', 1)
        .arg(dsp.midPercent, 0, 'f', 1).arg(dsp.highPercent, 0, 'f', 1), informationColor);
    addFinding(ui_->issuesList, messageFor(languageCode_, "Silence: %1 s leading · %2 s trailing", "Silencio: %1 s inicial · %2 s final")
        .arg(dsp.leadingSilenceSeconds, 0, 'f', 2).arg(dsp.trailingSilenceSeconds, 0, 'f', 2), informationColor);
    if (clipping) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "Clipping detected in %1 sample frames.", "Clipping detectado en %1 frames de muestras.").arg(clippedFrames), QColor("#c62828")); ++warnings; }
    if (peakDb > -1.0 && !clipping) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "Low headroom: peak is above −1 dBFS.", "Margen reducido: el pico supera −1 dBFS."), QColor("#b26a00")); ++warnings; }
    if (std::abs(leftDcOffset) > .01 || std::abs(rightDcOffset) > .01) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "Significant DC offset detected.", "Se ha detectado una componente DC significativa."), QColor("#b26a00")); ++warnings; }
    if (channels > 1 && std::abs(leftRmsDb - rightRmsDb) > 1.5) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "Stereo level imbalance exceeds 1.5 dB.", "El desequilibrio estéreo supera 1,5 dB."), QColor("#b26a00")); ++warnings; }
    if (silencePercent > 10.0) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "More than 10% of the file is digital silence.", "Más del 10 % del archivo es silencio digital."), QColor("#b26a00")); ++warnings; }
    if (sampleRate < 44100) { addFinding(ui_->issuesList,
        messageFor(languageCode_, "Sample rate is below 44.1 kHz.", "La frecuencia de muestreo es inferior a 44,1 kHz."), QColor("#b26a00")); ++warnings; }
    if (channels > 1 && (dsp.correlation < 0.15 || dsp.monoCompatibilityDb < -3.0)) {
        addFinding(ui_->issuesList, messageFor(languageCode_, "Possible mono compatibility problem.", "Posible problema de compatibilidad mono."), QColor("#b26a00"));
        ++warnings;
    }
    if (dsp.subsonicPercent > 1.0) {
        addFinding(ui_->issuesList, messageFor(languageCode_, "Significant energy below 20 Hz detected.", "Se ha detectado energía significativa por debajo de 20 Hz."), QColor("#b26a00"));
        ++warnings;
    }
    if (dsp.leadingSilenceSeconds > 2.0 || dsp.trailingSilenceSeconds > 2.0) {
        addFinding(ui_->issuesList, messageFor(languageCode_, "Long leading or trailing silence detected.", "Se ha detectado un silencio inicial o final prolongado."), QColor("#b26a00"));
        ++warnings;
    }
    if (dsp.bpm > 0.0 && dsp.bpmConfidence < 20.0) {
        addFinding(ui_->issuesList, messageFor(languageCode_, "BPM estimate has low confidence; treat it as uncertain.", "La estimación BPM tiene poca confianza; debe considerarse incierta."), QColor("#b26a00"));
        ++warnings;
    }
    if (warnings == 0) addFinding(ui_->issuesList,
        messageFor(languageCode_, "No obvious technical problems detected.", "No se han detectado problemas técnicos evidentes."), QColor("#27834a"));

    // Informe autocontenido: conserva las cifras, el método y conclusiones
    // derivadas de umbrales explícitos para poder copiarlo fuera de la app.
    const bool spanish = languageCode_ == "es";
    QStringList conclusions;
    if (clippedFrames > 0)
        conclusions << (spanish
            ? QStringLiteral("Se detecta clipping de muestras (%1 frames); conviene inspeccionar las zonas marcadas.").arg(clippedFrames)
            : QStringLiteral("Sample clipping is present (%1 frames); inspect the marked regions.").arg(clippedFrames));
    else if (dsp.truePeakDbtp <= -1.0)
        conclusions << (spanish ? QStringLiteral("El archivo conserva al menos 1 dB de margen true peak estimado.")
                                : QStringLiteral("The file retains at least 1 dB of estimated true-peak headroom."));
    else
        conclusions << (spanish ? QStringLiteral("El margen true peak es inferior a 1 dB; una conversión posterior podría producir sobrepicos.")
                                : QStringLiteral("True-peak headroom is below 1 dB; later conversion may produce overs."));
    const double crestFactor = std::isfinite(peakDb) && std::isfinite(referenceRms)
        ? peakDb - referenceRms : 0.0;
    if (crestFactor < 6.0)
        conclusions << (spanish ? QStringLiteral("El factor de cresta es bajo (%1 dB): el nivel medio está muy próximo a los picos, compatible con dinámica densa o limitación intensa.").arg(crestFactor, 0, 'f', 2)
                                : QStringLiteral("Crest factor is low (%1 dB): average level is close to peaks, consistent with dense dynamics or strong limiting.").arg(crestFactor, 0, 'f', 2));
    else if (crestFactor > 14.0)
        conclusions << (spanish ? QStringLiteral("El factor de cresta es alto (%1 dB): existen transitorios que sobresalen claramente del nivel medio.").arg(crestFactor, 0, 'f', 2)
                                : QStringLiteral("Crest factor is high (%1 dB): transients stand clearly above average level.").arg(crestFactor, 0, 'f', 2));
    else
        conclusions << (spanish ? QStringLiteral("El factor de cresta (%1 dB) se encuentra en una zona intermedia; no implica por sí solo un defecto.").arg(crestFactor, 0, 'f', 2)
                                : QStringLiteral("Crest factor (%1 dB) is in an intermediate range and does not by itself imply a fault.").arg(crestFactor, 0, 'f', 2));
    if (std::isfinite(dsp.integratedLufs)) {
        if (dsp.integratedLufs > -10.0)
            conclusions << (spanish ? QStringLiteral("La sonoridad integrada estimada es muy elevada; debe interpretarse junto con cresta, clipping y estilo del material.")
                                    : QStringLiteral("Estimated integrated loudness is very high; interpret it with crest factor, clipping and programme style."));
        else if (dsp.integratedLufs < -22.0)
            conclusions << (spanish ? QStringLiteral("La sonoridad integrada estimada es baja; esto puede ser intencional y no constituye un fallo.")
                                    : QStringLiteral("Estimated integrated loudness is low; this may be intentional and is not a fault."));
    }
    if (channels > 1 && (dsp.correlation < .15 || dsp.monoCompatibilityDb < -3.0))
        conclusions << (spanish ? QStringLiteral("La combinación de correlación y pérdida mono aconseja comprobar cancelaciones al sumar L y R.")
                                : QStringLiteral("Correlation and mono loss together warrant checking cancellation when L and R are summed."));
    else if (channels > 1)
        conclusions << (spanish ? QStringLiteral("No se observa una pérdida mono importante en la medición global.")
                                : QStringLiteral("No major mono loss is visible in the global measurement."));
    if (dsp.subsonicPercent > 1.0)
        conclusions << (spanish ? QStringLiteral("Existe energía relevante por debajo de 20 Hz, inaudible en la mayoría de sistemas pero capaz de consumir margen.")
                                : QStringLiteral("Relevant energy exists below 20 Hz; usually inaudible, but it can consume headroom."));
    if (std::abs(leftDcOffset) > .01 || std::abs(rightDcOffset) > .01)
        conclusions << (spanish ? QStringLiteral("La componente DC supera el umbral de advertencia y reduce el margen útil.")
                                : QStringLiteral("DC offset exceeds the warning threshold and reduces usable headroom."));
    if (dsp.bpm <= 0.0)
        conclusions << (spanish ? QStringLiteral("No se ofrece BPM porque la duración o la estructura no permiten una estimación responsable.")
                                : QStringLiteral("No BPM is reported because duration or structure does not support a responsible estimate."));

    QStringList report;
    report << QStringLiteral("AudioInspector Qt — %1").arg(spanish ? "Informe técnico" : "Technical report")
           << QStringLiteral("Copyright © 2026 Tomás Fernández Galera") << QString()
           << (spanish ? QStringLiteral("ARCHIVO") : QStringLiteral("FILE"))
           << QStringLiteral("  %1").arg(QFileInfo(path).fileName())
           << QStringLiteral("  %1").arg(QFileInfo(path).absoluteFilePath())
           << (spanish
                ? QStringLiteral("  %1 · %2 Hz · PCM decodificado de %3 bits · %4 canales · %5 kbps")
                : QStringLiteral("  %1 · %2 Hz · %3-bit decoded PCM · %4 channels · %5 kbps"))
                .arg(QFileInfo(path).suffix().toUpper()).arg(sampleRate).arg(bits).arg(channels).arg(bitrateKbps)
           << QStringLiteral("  %1:%2").arg(durationMs / 60000).arg((durationMs / 1000) % 60, 2, 10, QLatin1Char('0')) << QString()
           << (spanish ? QStringLiteral("NIVEL Y DINÁMICA") : QStringLiteral("LEVEL AND DYNAMICS"))
           << (spanish ? QStringLiteral("  Pico de muestra: %1") : QStringLiteral("  Sample peak: %1")).arg(decibels(peakDb))
           << (spanish ? QStringLiteral("  True peak (estimación 4x): %1 dBTP") : QStringLiteral("  True peak (4x estimate): %1 dBTP")).arg(dsp.truePeakDbtp, 0, 'f', 2)
           << (spanish ? QStringLiteral("  RMS izquierdo/derecho: %1 / %2") : QStringLiteral("  RMS L/R: %1 / %2")).arg(decibels(leftRmsDb), decibels(rightRmsDb))
           << (spanish ? QStringLiteral("  Factor de cresta: %1 dB") : QStringLiteral("  Crest factor: %1 dB")).arg(crestFactor, 0, 'f', 2)
           << (spanish ? QStringLiteral("  Frames recortados: %1") : QStringLiteral("  Clipped frames: %1")).arg(clippedFrames)
           << (spanish
                ? QStringLiteral("  Estimación LUFS — integrada: %1 · momentánea máxima: %2 · short-term máxima: %3")
                : QStringLiteral("  LUFS estimate — integrated: %1 · max momentary: %2 · max short-term: %3"))
                .arg(dsp.integratedLufs, 0, 'f', 1).arg(dsp.momentaryLufs, 0, 'f', 1).arg(dsp.shortTermLufs, 0, 'f', 1)
           << (spanish ? QStringLiteral("  Rango dinámico por secciones: %1 dB") : QStringLiteral("  Section dynamic range: %1 dB")).arg(dsp.dynamicRangeDb, 0, 'f', 1) << QString()
           << (spanish ? QStringLiteral("ESTÉREO Y ESPECTRO") : QStringLiteral("STEREO AND SPECTRUM"))
           << (spanish ? QStringLiteral("  Correlación: %1 · cambio mono: %2 dB") : QStringLiteral("  Correlation: %1 · mono change: %2 dB")).arg(dsp.correlation, 0, 'f', 3).arg(dsp.monoCompatibilityDb, 0, 'f', 2)
           << (spanish ? QStringLiteral("  DC izquierdo/derecho: %1% / %2%") : QStringLiteral("  DC L/R: %1% / %2%")).arg(leftDcOffset * 100, 0, 'f', 3).arg(rightDcOffset * 100, 0, 'f', 3)
           << (spanish
                ? QStringLiteral("  Subsónico/graves/medios/agudos: %1% / %2% / %3% / %4%")
                : QStringLiteral("  Subsonic/low/mid/high: %1% / %2% / %3% / %4%"))
                .arg(dsp.subsonicPercent, 0, 'f', 2).arg(dsp.lowPercent, 0, 'f', 1).arg(dsp.midPercent, 0, 'f', 1).arg(dsp.highPercent, 0, 'f', 1)
           << (spanish ? QStringLiteral("  Silencio inicial/final: %1 s / %2 s") : QStringLiteral("  Leading/trailing silence: %1 s / %2 s")).arg(dsp.leadingSilenceSeconds, 0, 'f', 2).arg(dsp.trailingSilenceSeconds, 0, 'f', 2)
           << (spanish ? QStringLiteral("  BPM: %1 · confianza: %2% · tonalidad estimada: %3") : QStringLiteral("  BPM: %1 · confidence: %2% · key estimate: %3"))
                .arg(dsp.bpm > 0 ? QString::number(dsp.bpm, 'f', 1) : QStringLiteral("N/A"))
                .arg(dsp.bpmConfidence, 0, 'f', 0).arg(displayedKey) << QString()
           << (spanish ? QStringLiteral("CONCLUSIONES") : QStringLiteral("CONCLUSIONS"));
    for (const QString &conclusion : conclusions) report << QStringLiteral("• %1").arg(conclusion);
    report << QString() << (spanish
        ? QStringLiteral("LIMITACIONES: LUFS, true peak, BPM y tonalidad son estimaciones. El informe describe el archivo digital y no mide DAC, amplificación, transductores, sala ni percepción.")
        : QStringLiteral("LIMITATIONS: LUFS, true peak, BPM and key are estimates. This report describes the digital file and does not measure DAC, amplification, transducers, room or perception."));
    lastReport_ = report.join(QLatin1Char('\n'));
    if (languageCode_ == "zh") {
        const QList<QPair<QString, QString>> replacements = {
            {"AudioInspector Qt — Technical report", "AudioInspector Qt — 技术报告"},
            {"FILE", "文件"}, {"LEVEL AND DYNAMICS", "电平与动态"},
            {"STEREO AND SPECTRUM", "立体声与频谱"}, {"CONCLUSIONS", "结论"},
            {"decoded PCM", "解码 PCM"}, {"channels", "声道"},
            {"Sample peak", "采样峰值"}, {"True peak (4x estimate)", "True Peak（4倍估算）"},
            {"RMS L/R", "RMS 左/右"}, {"Crest factor", "峰值因数"},
            {"Clipped frames", "削波帧"}, {"LUFS estimate", "LUFS 估算"},
            {"integrated", "综合"}, {"max momentary", "最大瞬时"},
            {"max short-term", "最大短时"}, {"Section dynamic range", "分段动态范围"},
            {"Correlation", "相关度"}, {"mono change", "单声道变化"},
            {"DC L/R", "DC 左/右"}, {"Subsonic/low/mid/high", "次声/低频/中频/高频"},
            {"Leading/trailing silence", "开头/结尾静音"}, {"confidence", "置信度"},
            {"key estimate", "估算调性"}, {" major", " 大调"}, {" minor", " 小调"},
            {"The file retains at least 1 dB of estimated true-peak headroom.", "文件保留至少 1 dB 的估算 True Peak 余量。"},
            {"True-peak headroom is below 1 dB; later conversion may produce overs.", "True Peak 余量低于 1 dB；后续转换可能产生采样间过载。"},
            {"Estimated integrated loudness is very high; interpret it with crest factor, clipping and programme style.", "估算综合响度很高；应结合峰值因数、削波和节目类型解释。"},
            {"Estimated integrated loudness is low; this may be intentional and is not a fault.", "估算综合响度较低；这可能是有意的，并不代表故障。"},
            {"Correlation and mono loss together warrant checking cancellation when L and R are summed.", "相关度与单声道损失表明，应检查左右声道相加时的抵消。"},
            {"No major mono loss is visible in the global measurement.", "全局测量未显示明显的单声道损失。"},
            {"Relevant energy exists below 20 Hz; usually inaudible, but it can consume headroom.", "存在明显的 20 Hz 以下能量；通常不可听，但会占用余量。"},
            {"DC offset exceeds the warning threshold and reduces usable headroom.", "直流偏移超过警告阈值，并减少可用余量。"},
            {"No BPM is reported because duration or structure does not support a responsible estimate.", "由于时长或结构不足，未给出不可靠的 BPM 估算。"},
            {"LIMITATIONS: LUFS, true peak, BPM and key are estimates. This report describes the digital file and does not measure DAC, amplification, transducers, room or perception.",
             "局限：LUFS、True Peak、BPM 和调性均为估算。本报告描述数字文件，不测量 DAC、放大器、换能器、房间或听觉感知。"}
        };
        for (const auto &replacement : replacements)
            lastReport_.replace(replacement.first, replacement.second);
        lastReport_.replace(
            QStringLiteral("Sample clipping is present (%1 frames); inspect the marked regions.").arg(clippedFrames),
            QStringLiteral("检测到采样削波（%1 帧）；应检查标记区域。").arg(clippedFrames));
        lastReport_.replace(
            QStringLiteral("Crest factor is low (%1 dB): average level is close to peaks, consistent with dense dynamics or strong limiting.").arg(crestFactor, 0, 'f', 2),
            QStringLiteral("峰值因数较低（%1 dB）：平均电平接近峰值，符合动态密集或强限制特征。").arg(crestFactor, 0, 'f', 2));
        lastReport_.replace(
            QStringLiteral("Crest factor is high (%1 dB): transients stand clearly above average level.").arg(crestFactor, 0, 'f', 2),
            QStringLiteral("峰值因数较高（%1 dB）：瞬态明显高于平均电平。").arg(crestFactor, 0, 'f', 2));
        lastReport_.replace(
            QStringLiteral("Crest factor (%1 dB) is in an intermediate range and does not by itself imply a fault.").arg(crestFactor, 0, 'f', 2),
            QStringLiteral("峰值因数（%1 dB）处于中间范围，本身不表示故障。").arg(crestFactor, 0, 'f', 2));
    }
    reportButton_->setEnabled(true);

    ui_->analysisStatusLabel->setText(messageFor(languageCode_, "Analysis complete · %1 finding(s)", "Análisis completo · %1 aviso(s)").arg(warnings));
    statusBar()->showMessage(messageFor(languageCode_, "Analysis complete", "Análisis completo"));
    updateWaveformScale();
}

void MainWindow::updateWaveformScale()
{
    // El deslizador recorre exponencialmente desde una muestra visual por píxel
    // hasta archivo completo. La progresión evita que casi todo el recorrido se
    // concentre en uno de los extremos para canciones largas.
    const int viewportWidth = qMax(1, ui_->waveformScrollArea->viewport()->width());
    constexpr int margins = 44;
    const int oneToOneWidth = qMax(viewportWidth, currentSampleCount_ + margins);
    const double fitRatio = qMax(1.0, oneToOneWidth / double(viewportWidth));
    const double compression = std::pow(fitRatio, ui_->zoomSlider->value() / 1000.0);
    const int newWidth = qMax(viewportWidth, qRound(oneToOneWidth / compression));
    const int viewportHeight = qMax(190, ui_->waveformScrollArea->viewport()->height());
    ui_->waveformScrollContents->resize(newWidth, viewportHeight);
    waveform_->resize(newWidth, viewportHeight);
    if (currentSampleCount_ == 0) ui_->zoomValueLabel->setText(QStringLiteral("—"));
    else if (ui_->zoomSlider->value() == 0) ui_->zoomValueLabel->setText(QStringLiteral("1:1"));
    else if (ui_->zoomSlider->value() == 1000) ui_->zoomValueLabel->setText(QStringLiteral("Full file"));
    else ui_->zoomValueLabel->setText(QStringLiteral("1:%1").arg(compression, 0, 'f', 1));
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
