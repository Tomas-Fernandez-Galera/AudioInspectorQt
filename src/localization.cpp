#include "localization.h"

#include <QHash>

namespace Localization {
QString text(const QString &key, const QString &language)
{
    // Las tablas viven fuera de MainWindow para que añadir idiomas no altere el
    // flujo de análisis ni obligue a buscar cadenas por todo el código.
    static const QHash<QString, QString> es = {
        {"open", "Abrir archivo de audio…"}, {"help", "Ayuda"}, {"close", "Cerrar"},
        {"timeScale", "Escala temporal"}, {"samples", "Muestras 1:1"},
        {"fullFile", "Archivo completo"}, {"dark", "Tema oscuro"},
        {"play", "▶ Reproducir"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Parar"},
        {"volume", "Volumen"}, {"technical", "Análisis técnico"},
        {"advanced", "Análisis avanzado y hallazgos"}, {"format", "Formato"},
        {"sampleRate", "Frecuencia de muestreo"}, {"depth", "Profundidad de bits"},
        {"channels", "Canales"}, {"duration", "Duración / bitrate"},
        {"peak", "Pico de muestra"}, {"rmsLeft", "RMS izquierdo"},
        {"rmsRight", "RMS derecho"}, {"dc", "Componente DC I / D"},
        {"silence", "Silencio digital"}, {"crest", "Factor de cresta"},
        {"clipped", "Frames recortados"}, {"noFile", "Ningún archivo cargado"},
        {"choose", "Elige un archivo para iniciar un análisis nuevo"},
        {"ready", "Preparado"}, {"spectrogram", "El espectrograma aparecerá después del análisis"},
        {"spectrum", "El espectro FFT aparecerá después del análisis"}
    };
    static const QHash<QString, QString> en = {
        {"open", "Open audio file…"}, {"help", "Help"}, {"close", "Close"},
        {"timeScale", "Time scale"}, {"samples", "1:1 samples"},
        {"fullFile", "Full file"}, {"dark", "Dark theme"},
        {"play", "▶ Play"}, {"pause", "❚❚ Pause"}, {"stop", "■ Stop"},
        {"volume", "Volume"}, {"technical", "Technical analysis"},
        {"advanced", "Advanced analysis and findings"}, {"format", "Format"},
        {"sampleRate", "Sample rate"}, {"depth", "Bit depth"},
        {"channels", "Channels"}, {"duration", "Duration / bitrate"},
        {"peak", "Sample peak"}, {"rmsLeft", "RMS left"},
        {"rmsRight", "RMS right"}, {"dc", "DC offset L / R"},
        {"silence", "Digital silence"}, {"crest", "Crest factor"},
        {"clipped", "Clipped frames"}, {"noFile", "No file loaded"},
        {"choose", "Choose a file to start a new analysis"}, {"ready", "Ready"},
        {"spectrogram", "Spectrogram appears after analysis"},
        {"spectrum", "FFT spectrum appears after analysis"}
    };
    static const QHash<QString, QString> ca = {
        {"open", "Obre un fitxer d’àudio…"}, {"help", "Ajuda"}, {"close", "Tanca"}, {"timeScale", "Escala temporal"},
        {"samples", "Mostres 1:1"}, {"fullFile", "Fitxer complet"}, {"dark", "Tema fosc"},
        {"play", "▶ Reprodueix"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Atura"}, {"volume", "Volum"},
        {"technical", "Anàlisi tècnica"}, {"advanced", "Anàlisi avançada i resultats"},
        {"format", "Format"}, {"sampleRate", "Freqüència de mostreig"}, {"depth", "Profunditat de bits"},
        {"channels", "Canals"}, {"duration", "Durada / bitrate"}, {"peak", "Pic de mostra"},
        {"rmsLeft", "RMS esquerre"}, {"rmsRight", "RMS dret"}, {"dc", "Component DC E / D"},
        {"silence", "Silenci digital"}, {"crest", "Factor de cresta"}, {"clipped", "Frames retallats"},
        {"noFile", "Cap fitxer carregat"}, {"choose", "Tria un fitxer per iniciar una anàlisi nova"},
        {"ready", "Preparat"}, {"spectrogram", "L’espectrograma apareixerà després de l’anàlisi"},
        {"spectrum", "L’espectre FFT apareixerà després de l’anàlisi"}
    };
    static const QHash<QString, QString> gl = {
        {"open", "Abrir ficheiro de audio…"}, {"help", "Axuda"}, {"close", "Pechar"}, {"timeScale", "Escala temporal"},
        {"samples", "Mostras 1:1"}, {"fullFile", "Ficheiro completo"}, {"dark", "Tema escuro"},
        {"play", "▶ Reproducir"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Deter"}, {"volume", "Volume"},
        {"technical", "Análise técnica"}, {"advanced", "Análise avanzada e achados"},
        {"format", "Formato"}, {"sampleRate", "Frecuencia de mostraxe"}, {"depth", "Profundidade de bits"},
        {"channels", "Canles"}, {"duration", "Duración / bitrate"}, {"peak", "Pico de mostra"},
        {"rmsLeft", "RMS esquerdo"}, {"rmsRight", "RMS dereito"}, {"dc", "Compoñente DC E / D"},
        {"silence", "Silencio dixital"}, {"crest", "Factor de crista"}, {"clipped", "Frames recortados"},
        {"noFile", "Ningún ficheiro cargado"}, {"choose", "Escolle un ficheiro para iniciar unha análise nova"},
        {"ready", "Preparado"}, {"spectrogram", "O espectrograma aparecerá despois da análise"},
        {"spectrum", "O espectro FFT aparecerá despois da análise"}
    };
    static const QHash<QString, QString> eu = {
        {"open", "Ireki audio-fitxategia…"}, {"help", "Laguntza"}, {"close", "Itxi"}, {"timeScale", "Denbora-eskala"},
        {"samples", "1:1 laginak"}, {"fullFile", "Fitxategi osoa"}, {"dark", "Gai iluna"},
        {"play", "▶ Erreproduzitu"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Gelditu"}, {"volume", "Bolumena"},
        {"technical", "Azterketa teknikoa"}, {"advanced", "Azterketa aurreratua eta aurkikuntzak"},
        {"format", "Formatua"}, {"sampleRate", "Laginketa-maiztasuna"}, {"depth", "Bit-sakonera"},
        {"channels", "Kanalak"}, {"duration", "Iraupena / bitratea"}, {"peak", "Lagin-gailurra"},
        {"rmsLeft", "Ezkerreko RMS"}, {"rmsRight", "Eskuineko RMS"}, {"dc", "DC osagaia E / E"},
        {"silence", "Isiltasun digitala"}, {"crest", "Gandor-faktorea"}, {"clipped", "Moztutako frameak"},
        {"noFile", "Ez dago fitxategirik kargatuta"}, {"choose", "Aukeratu fitxategi bat azterketa hasteko"},
        {"ready", "Prest"}, {"spectrogram", "Espektrograma azterketaren ondoren agertuko da"},
        {"spectrum", "FFT espektroa azterketaren ondoren agertuko da"}
    };
    static const QHash<QString, QString> fr = {
        {"open", "Ouvrir un fichier audio…"}, {"help", "Aide"}, {"close", "Fermer"},
        {"timeScale", "Échelle temporelle"}, {"samples", "Échantillons 1:1"}, {"fullFile", "Fichier entier"},
        {"dark", "Thème sombre"}, {"play", "▶ Lire"}, {"pause", "❚❚ Pause"}, {"stop", "■ Arrêter"},
        {"volume", "Volume"}, {"technical", "Analyse technique"}, {"advanced", "Analyse avancée et résultats"},
        {"format", "Format"}, {"sampleRate", "Fréquence d’échantillonnage"}, {"depth", "Résolution en bits"},
        {"channels", "Canaux"}, {"duration", "Durée / débit"}, {"peak", "Crête d’échantillon"},
        {"rmsLeft", "RMS gauche"}, {"rmsRight", "RMS droit"}, {"dc", "Décalage DC G / D"},
        {"silence", "Silence numérique"}, {"crest", "Facteur de crête"}, {"clipped", "Trames écrêtées"},
        {"noFile", "Aucun fichier chargé"}, {"choose", "Choisissez un fichier pour lancer une nouvelle analyse"},
        {"ready", "Prêt"}, {"spectrogram", "Le spectrogramme apparaîtra après l’analyse"},
        {"spectrum", "Le spectre FFT apparaîtra après l’analyse"}
    };
    static const QHash<QString, QString> de = {
        {"open", "Audiodatei öffnen…"}, {"help", "Hilfe"}, {"close", "Schließen"},
        {"timeScale", "Zeitskala"}, {"samples", "1:1 Samples"}, {"fullFile", "Ganze Datei"},
        {"dark", "Dunkles Design"}, {"play", "▶ Wiedergabe"}, {"pause", "❚❚ Pause"}, {"stop", "■ Stopp"},
        {"volume", "Lautstärke"}, {"technical", "Technische Analyse"}, {"advanced", "Erweiterte Analyse und Befunde"},
        {"format", "Format"}, {"sampleRate", "Abtastrate"}, {"depth", "Bittiefe"}, {"channels", "Kanäle"},
        {"duration", "Dauer / Bitrate"}, {"peak", "Sample-Spitze"}, {"rmsLeft", "RMS links"},
        {"rmsRight", "RMS rechts"}, {"dc", "DC-Versatz L / R"}, {"silence", "Digitale Stille"},
        {"crest", "Crest-Faktor"}, {"clipped", "Übersteuerte Frames"}, {"noFile", "Keine Datei geladen"},
        {"choose", "Datei auswählen, um eine neue Analyse zu starten"}, {"ready", "Bereit"},
        {"spectrogram", "Das Spektrogramm erscheint nach der Analyse"},
        {"spectrum", "Das FFT-Spektrum erscheint nach der Analyse"}
    };
    static const QHash<QString, QString> it = {
        {"open", "Apri file audio…"}, {"help", "Aiuto"}, {"close", "Chiudi"},
        {"timeScale", "Scala temporale"}, {"samples", "Campioni 1:1"}, {"fullFile", "File completo"},
        {"dark", "Tema scuro"}, {"play", "▶ Riproduci"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Arresta"},
        {"volume", "Volume"}, {"technical", "Analisi tecnica"}, {"advanced", "Analisi avanzata e risultati"},
        {"format", "Formato"}, {"sampleRate", "Frequenza di campionamento"}, {"depth", "Profondità in bit"},
        {"channels", "Canali"}, {"duration", "Durata / bitrate"}, {"peak", "Picco del campione"},
        {"rmsLeft", "RMS sinistro"}, {"rmsRight", "RMS destro"}, {"dc", "Offset DC S / D"},
        {"silence", "Silenzio digitale"}, {"crest", "Fattore di cresta"}, {"clipped", "Frame in clipping"},
        {"noFile", "Nessun file caricato"}, {"choose", "Scegli un file per iniziare una nuova analisi"},
        {"ready", "Pronto"}, {"spectrogram", "Lo spettrogramma apparirà dopo l’analisi"},
        {"spectrum", "Lo spettro FFT apparirà dopo l’analisi"}
    };
    static const QHash<QString, QString> pt = {
        {"open", "Abrir ficheiro de áudio…"}, {"help", "Ajuda"}, {"close", "Fechar"},
        {"timeScale", "Escala temporal"}, {"samples", "Amostras 1:1"}, {"fullFile", "Ficheiro completo"},
        {"dark", "Tema escuro"}, {"play", "▶ Reproduzir"}, {"pause", "❚❚ Pausa"}, {"stop", "■ Parar"},
        {"volume", "Volume"}, {"technical", "Análise técnica"}, {"advanced", "Análise avançada e resultados"},
        {"format", "Formato"}, {"sampleRate", "Frequência de amostragem"}, {"depth", "Profundidade de bits"},
        {"channels", "Canais"}, {"duration", "Duração / bitrate"}, {"peak", "Pico de amostra"},
        {"rmsLeft", "RMS esquerdo"}, {"rmsRight", "RMS direito"}, {"dc", "Componente DC E / D"},
        {"silence", "Silêncio digital"}, {"crest", "Fator de crista"}, {"clipped", "Frames recortados"},
        {"noFile", "Nenhum ficheiro carregado"}, {"choose", "Escolha um ficheiro para iniciar uma nova análise"},
        {"ready", "Pronto"}, {"spectrogram", "O espectrograma aparecerá após a análise"},
        {"spectrum", "O espectro FFT aparecerá após a análise"}
    };
    static const QHash<QString, QString> ja = {
        {"open", "オーディオファイルを開く…"}, {"help", "ヘルプ"}, {"close", "閉じる"},
        {"timeScale", "時間スケール"}, {"samples", "1:1 サンプル"}, {"fullFile", "ファイル全体"},
        {"dark", "ダークテーマ"}, {"play", "▶ 再生"}, {"pause", "❚❚ 一時停止"}, {"stop", "■ 停止"},
        {"volume", "音量"}, {"technical", "技術分析"}, {"advanced", "高度な分析と検出結果"},
        {"format", "形式"}, {"sampleRate", "サンプルレート"}, {"depth", "ビット深度"}, {"channels", "チャンネル"},
        {"duration", "長さ / ビットレート"}, {"peak", "サンプルピーク"}, {"rmsLeft", "左 RMS"},
        {"rmsRight", "右 RMS"}, {"dc", "DC オフセット L / R"}, {"silence", "デジタル無音"},
        {"crest", "クレストファクター"}, {"clipped", "クリップフレーム"}, {"noFile", "ファイルが読み込まれていません"},
        {"choose", "新しい分析を開始するファイルを選択してください"}, {"ready", "準備完了"},
        {"spectrogram", "分析後にスペクトログラムが表示されます"}, {"spectrum", "分析後に FFT スペクトルが表示されます"}
    };
    static const QHash<QString, QString> ko = {
        {"open", "오디오 파일 열기…"}, {"help", "도움말"}, {"close", "닫기"},
        {"timeScale", "시간 배율"}, {"samples", "1:1 샘플"}, {"fullFile", "전체 파일"},
        {"dark", "어두운 테마"}, {"play", "▶ 재생"}, {"pause", "❚❚ 일시 정지"}, {"stop", "■ 정지"},
        {"volume", "볼륨"}, {"technical", "기술 분석"}, {"advanced", "고급 분석 및 결과"},
        {"format", "형식"}, {"sampleRate", "샘플 레이트"}, {"depth", "비트 깊이"}, {"channels", "채널"},
        {"duration", "길이 / 비트레이트"}, {"peak", "샘플 피크"}, {"rmsLeft", "왼쪽 RMS"},
        {"rmsRight", "오른쪽 RMS"}, {"dc", "DC 오프셋 L / R"}, {"silence", "디지털 무음"},
        {"crest", "크레스트 팩터"}, {"clipped", "클리핑 프레임"}, {"noFile", "불러온 파일 없음"},
        {"choose", "새 분석을 시작할 파일을 선택하세요"}, {"ready", "준비됨"},
        {"spectrogram", "분석 후 스펙트로그램이 표시됩니다"}, {"spectrum", "분석 후 FFT 스펙트럼이 표시됩니다"}
    };
    static const QHash<QString, QString> zh = {
        {"open", "打开音频文件…"}, {"help", "帮助"}, {"close", "关闭"},
        {"timeScale", "时间比例"}, {"samples", "1:1 采样"}, {"fullFile", "完整文件"},
        {"dark", "深色主题"}, {"play", "▶ 播放"}, {"pause", "❚❚ 暂停"}, {"stop", "■ 停止"},
        {"volume", "音量"}, {"technical", "技术分析"}, {"advanced", "高级分析与检测结果"},
        {"format", "格式"}, {"sampleRate", "采样率"}, {"depth", "位深"}, {"channels", "声道"},
        {"duration", "时长 / 比特率"}, {"peak", "采样峰值"}, {"rmsLeft", "左声道 RMS"},
        {"rmsRight", "右声道 RMS"}, {"dc", "直流偏移 L / R"}, {"silence", "数字静音"},
        {"crest", "峰值因数"}, {"clipped", "削波帧"}, {"noFile", "未加载文件"},
        {"choose", "选择文件以开始新的分析"}, {"ready", "就绪"},
        {"spectrogram", "分析后将显示频谱图"}, {"spectrum", "分析后将显示 FFT 频谱"}
    };
    const QHash<QString, QString> *table = &en;
    if (language == "es") table = &es;
    else if (language == "ca") table = &ca;
    else if (language == "gl") table = &gl;
    else if (language == "eu") table = &eu;
    else if (language == "fr") table = &fr;
    else if (language == "de") table = &de;
    else if (language == "it") table = &it;
    else if (language == "pt") table = &pt;
    else if (language == "ja") table = &ja;
    else if (language == "ko") table = &ko;
    else if (language == "zh") table = &zh;
    return table->value(key, en.value(key, key));
}

QString helpHtml(const QString &language)
{
    // La ayuda es HTML estático deliberadamente: resulta portable, seleccionable
    // y admite listas/jerarquía sin introducir un motor web.
    if (language == "fr") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — Comprendre l’analyse</h1><p>L’application analyse un fichier sans le modifier. Aucune valeur isolée ne juge la qualité : il faut réunir mesures et écoute.</p><h2>Forme d’onde</h2><p>Le temps va de gauche à droite et l’amplitude est verticale. Le mode 1:1 montre le signal comme un oscilloscope; la vue éloignée utilise les vrais minimums et maximums pour éviter l’aliasing. Un transitoire normal présente une attaque puis une décroissance.</p><h2>Niveau, dBFS et true peak</h2><p>0 dBFS est le plafond numérique; −6 dBFS laisse de la marge et −1 dBFS en laisse peu. Le sample peak est la plus grande valeur stockée. Le true peak 4× estime les dépassements entre échantillons; rester vers −1 dBTP facilite les conversions. Le nombre de trames écrêtées indique combien de positions touchent presque le plafond.</p><h2>RMS, crête et dynamique</h2><p>Le RMS décrit l’énergie moyenne et permet de comparer gauche et droite. Le facteur de crête est la différence entre pic et RMS : faible signifie souvent un signal plus compressé. La dynamique compare des fenêtres de trois secondes et révèle la variation entre passages calmes et forts.</p><h2>LUFS</h2><p>Plus la valeur approche zéro, plus le son est fort. L’intégrée résume le morceau, la momentary maximale mesure 400 ms et la short-term maximale 3 s. À titre indicatif : −23 LUFS doux, −18 modéré, −14 fort, −9 très fort. Ce sont des estimations et non un compteur EBU R128 certifié.</p><h2>Stéréo</h2><p>Une corrélation proche de +1 indique des canaux similaires; proche de 0 une image large; négative un risque d’annulation en mono. Interprétez-la avec la variation de niveau mono.</p><h2>FFT et spectrogramme</h2><p>La FFT place les graves à gauche et les aigus à droite. Le spectrogramme ajoute le temps et représente l’énergie par la couleur. Ils aident à repérer ronflement, sifflement, bruit, perte d’aigus et énergie sous 20 Hz. La couleur est relative au fichier.</p><h2>DC, BPM, tonalité et silence</h2><p>Un décalage DC réduit la marge. Le BPM est accompagné d’une confiance et peut confondre tempo, moitié et double. La tonalité est indicative, notamment en cas de modulation. Les silences de début et de fin signalent une exportation éventuellement mal découpée.</p><h2>Méthode</h2><ol><li>Écoutez.</li><li>Vérifiez écrêtage, true peak et DC.</li><li>Comparez LUFS, RMS et facteur de crête.</li><li>Contrôlez stéréo et mono.</li><li>Localisez les défauts dans l’onde, la FFT et le spectrogramme.</li></ol>)HTML");
    if (language == "de") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — Messwerte verstehen</h1><p>Die Anwendung untersucht eine Datei, ohne sie zu verändern. Kein einzelner Wert entscheidet über Qualität; Messung und kritisches Hören gehören zusammen.</p><h2>Wellenform</h2><p>Zeit läuft von links nach rechts, Amplitude vertikal. 1:1 zeigt eine Oszilloskopspur; beim Herauszoomen bewahrt eine Min/Max-Hüllkurve die echten Spitzen ohne Aliasing. Ein Schlagzeugimpuls besitzt meist einen schnellen Angriff und ein abklingendes Schwingen.</p><h2>dBFS, Sample Peak und True Peak</h2><p>0 dBFS ist die digitale Obergrenze. −6 dBFS bietet Reserve, −1 dBFS liegt dicht am Limit. Sample Peak ist der höchste gespeicherte Wert. Die 4×-True-Peak-Schätzung sucht Spitzen zwischen Samples; etwa −1 dBTP Reserve ist für Umwandlungen sinnvoll. Viele übersteuerte Frames deuten auf echtes Clipping.</p><h2>RMS, Crest-Faktor und Dynamik</h2><p>RMS beschreibt die mittlere Energie pro Kanal. Dauerhafte L/R-Unterschiede können ein Ungleichgewicht anzeigen. Crest-Faktor ist Peak minus RMS: kleine Werte sprechen oft für starke Kompression. Die Abschnittsdynamik vergleicht Drei-Sekunden-Fenster.</p><h2>LUFS</h2><p>Je näher an Null, desto lauter. Integrated beschreibt die ganze Datei, Max Momentary 400 ms und Max Short-term 3 s. Grobe Orientierung: −23 LUFS leise, −18 mittel, −14 laut, −9 sehr laut. Die Werte sind Schätzungen, keine zertifizierte EBU-R128-Messung.</p><h2>Stereo und Mono</h2><p>Korrelation nahe +1 bedeutet ähnliche Kanäle, nahe 0 breite Stereoinformation und unter 0 mögliche Auslöschung in Mono. Zusammen mit der Mono-Pegeländerung bewerten.</p><h2>FFT und Spektrogramm</h2><p>Die FFT zeigt Bass links und Höhen rechts. Das Spektrogramm ergänzt Zeit und Farbstärke. Damit lassen sich Brummen, Pfeifen, kurze Störungen, fehlende Höhen und Energie unter 20 Hz finden. Farben sind relativ zur jeweiligen Datei.</p><h2>DC, BPM, Tonart und Stille</h2><p>DC-Versatz verschiebt das Signal und kostet Headroom. BPM enthält einen Vertrauenswert und kann Halb-/Doppeltempo verwechseln. Die Tonart ist eine Schätzung. Lange Anfangs- oder Endstille kann auf ungenauen Export hinweisen.</p><h2>Empfohlener Ablauf</h2><ol><li>Zuerst hören.</li><li>Clipping, True Peak und DC prüfen.</li><li>LUFS, RMS und Crest vergleichen.</li><li>Stereo/Mono gemeinsam bewerten.</li><li>Probleme in Welle, FFT und Spektrogramm lokalisieren.</li></ol>)HTML");
    if (language == "it") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — Capire l’analisi</h1><p>L’applicazione analizza un file senza modificarlo. Nessun numero, da solo, stabilisce la qualità: le misure vanno unite all’ascolto.</p><h2>Forma d’onda</h2><p>Il tempo procede da sinistra a destra e l’ampiezza è verticale. La vista 1:1 mostra la traccia da oscilloscopio; allontanandosi si usa l’inviluppo minimo/massimo per evitare aliasing. Un colpo di batteria normale ha un attacco rapido e un’oscillazione che decade.</p><h2>dBFS, picco e true peak</h2><p>0 dBFS è il limite digitale. −6 dBFS lascia margine, −1 dBFS è vicino al limite. Il sample peak è il campione memorizzato più alto. Il true peak 4× stima i picchi tra campioni; circa −1 dBTP offre margine nelle conversioni. Molti frame al limite indicano clipping.</p><h2>RMS, fattore di cresta e dinamica</h2><p>RMS rappresenta l’energia media di ogni canale. Differenze L/R persistenti possono indicare squilibrio. Il fattore di cresta è picco meno RMS: valori bassi spesso significano maggiore compressione. La dinamica confronta sezioni di tre secondi.</p><h2>LUFS</h2><p>Più il valore è vicino a zero, maggiore è la sonorità. Integrated descrive tutto il file, max momentary 400 ms e max short-term 3 s. Indicativamente: −23 LUFS delicato, −18 moderato, −14 forte, −9 molto forte. Sono stime, non misure EBU R128 certificate.</p><h2>Stereo, FFT e spettrogramma</h2><p>Correlazione +1 indica canali simili, 0 stereo ampio e valori negativi possibile cancellazione mono. La FFT mostra bassi a sinistra e alti a destra; lo spettrogramma aggiunge tempo e colore. Servono a trovare ronzii, rumori, perdita di alte e contenuto subsonico.</p><h2>DC, BPM, tonalità e silenzi</h2><p>L’offset DC riduce il margine. Il BPM ha una confidenza e può confondere tempo, metà e doppio. La tonalità è indicativa. Silenzi iniziali o finali lunghi possono derivare da un’esportazione imprecisa.</p><h2>Procedura</h2><ol><li>Ascolta.</li><li>Controlla clipping, true peak e DC.</li><li>Confronta LUFS, RMS e cresta.</li><li>Verifica stereo e mono.</li><li>Localizza i problemi con onda, FFT e spettrogramma.</li></ol>)HTML");
    if (language == "pt") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — Compreender a análise</h1><p>A aplicação analisa um ficheiro sem o modificar. Nenhum valor isolado determina a qualidade; combine medições com audição crítica.</p><h2>Forma de onda</h2><p>O tempo avança da esquerda para a direita e a amplitude é vertical. A vista 1:1 mostra o traço de osciloscópio; ao afastar, a envolvente mínimo/máximo evita aliasing. Um ataque de bateria costuma ter início rápido e oscilação decrescente.</p><h2>dBFS, pico e true peak</h2><p>0 dBFS é o teto digital. −6 dBFS deixa margem e −1 dBFS fica próximo do limite. Sample peak é a maior amostra guardada. True peak 4× estima picos entre amostras; cerca de −1 dBTP é uma margem útil para conversões. Muitos frames no limite indicam clipping.</p><h2>RMS, fator de crista e dinâmica</h2><p>RMS descreve a energia média de cada canal. Diferenças L/R persistentes podem revelar desequilíbrio. O fator de crista é pico menos RMS: valores baixos costumam indicar mais compressão. A dinâmica compara secções de três segundos.</p><h2>LUFS</h2><p>Quanto mais perto de zero, mais alto. Integrated resume o ficheiro, max momentary usa 400 ms e max short-term 3 s. Como orientação: −23 LUFS suave, −18 moderado, −14 forte, −9 muito forte. São estimativas, não medições EBU R128 certificadas.</p><h2>Estéreo, FFT e espectrograma</h2><p>Correlação +1 significa canais semelhantes, 0 estéreo amplo e valores negativos possível cancelamento mono. A FFT mostra graves à esquerda e agudos à direita; o espectrograma acrescenta tempo e cor. Ajudam a localizar zumbidos, ruído, falta de agudos e energia subsónica.</p><h2>DC, BPM, tonalidade e silêncio</h2><p>DC desloca o sinal e reduz a margem. BPM inclui confiança e pode confundir tempo, metade e dobro. A tonalidade é orientativa. Silêncios longos nas extremidades podem indicar exportação mal recortada.</p><h2>Método</h2><ol><li>Ouça primeiro.</li><li>Verifique clipping, true peak e DC.</li><li>Compare LUFS, RMS e crista.</li><li>Avalie estéreo e mono.</li><li>Localize problemas na onda, FFT e espectrograma.</li></ol>)HTML");
    if (language == "ja") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — 解析結果の読み方</h1><p>アプリはファイルを変更せずに解析します。一つの数値だけで音質は決まりません。測定結果と実際の試聴を組み合わせて判断してください。</p><h2>波形</h2><p>横軸は時間、縦軸は振幅です。1:1 はオシロスコープのような実波形を表示し、縮小時はエイリアシングを避けるため区間ごとの最小値と最大値を表示します。ドラムの打撃は通常、鋭い立ち上がりと減衰する振動を持ちます。</p><h2>dBFS・Sample Peak・True Peak</h2><p>0 dBFS がデジタル上限です。−6 dBFS には余裕があり、−1 dBFS は上限に近い値です。Sample Peak は保存された最大サンプル、4× True Peak はサンプル間ピークの推定です。変換用にはおよそ −1 dBTP 以下が目安です。上限に連続して触れる場合はクリッピングを疑います。</p><h2>RMS・クレストファクター・ダイナミクス</h2><p>RMS は各チャンネルの平均エネルギーです。左右差が継続する場合はバランスを確認します。クレストファクターは Peak と RMS の差で、小さい値は強い圧縮を示すことがあります。区間ダイナミクスは3秒ごとの静かな部分と大きな部分を比較します。</p><h2>LUFS</h2><p>ゼロに近いほど大きく聞こえます。Integrated は全体、Max Momentary は最も大きい400 ms、Max Short-term は最も大きい3秒です。目安は −23 LUFS が小さめ、−18 が中程度、−14 が大きめ、−9 が非常に大きい状態です。本アプリの値は推定で、認証済み EBU R128 メーターではありません。</p><h2>ステレオ・FFT・スペクトログラム</h2><p>相関 +1 は左右が似ており、0 は広いステレオ、負値はモノラル化で打ち消しの可能性があります。FFT は左に低音、右に高音を表示します。スペクトログラムは時間・周波数・エネルギーを示し、ハム、ノイズ、高域欠落、20 Hz 未満の不要成分を探せます。</p><h2>DC・BPM・キー・無音</h2><p>DC オフセットはヘッドルームを減らします。BPM の信頼度が低い場合や、半分・倍のテンポになる場合があります。キー推定は転調や特殊な調律では外れることがあります。長い先頭・末尾無音は書き出し範囲を確認してください。</p><h2>確認手順</h2><ol><li>まず聴く。</li><li>Clipping、True Peak、DC を確認。</li><li>LUFS、RMS、Crest を比較。</li><li>ステレオとモノ互換を確認。</li><li>波形、FFT、スペクトログラムで場所を特定。</li></ol>)HTML");
    if (language == "ko") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — 분석 결과 이해하기</h1><p>앱은 파일을 변경하지 않고 분석합니다. 하나의 수치만으로 품질을 판단할 수 없으므로 측정값과 청취를 함께 사용하세요.</p><h2>파형</h2><p>가로축은 시간, 세로축은 진폭입니다. 1:1은 오실로스코프 추적을 보여 주며, 축소 시에는 에일리어싱을 막기 위해 구간의 실제 최소/최대값을 표시합니다. 드럼 타격은 보통 빠른 어택 뒤에 감소하는 진동이 이어집니다.</p><h2>dBFS, Sample Peak, True Peak</h2><p>0 dBFS가 디지털 상한입니다. −6 dBFS는 여유가 있고 −1 dBFS는 상한에 가깝습니다. Sample Peak는 저장된 최고 샘플이며 4× True Peak는 샘플 사이 피크를 추정합니다. 변환을 위해 약 −1 dBTP 이하의 여유가 유용합니다. 많은 프레임이 상한에 닿으면 클리핑을 의심하세요.</p><h2>RMS, 크레스트 팩터, 다이내믹</h2><p>RMS는 채널별 평균 에너지입니다. 지속적인 좌우 차이는 불균형일 수 있습니다. 크레스트 팩터는 Peak와 RMS의 차이이며 작은 값은 강한 압축을 뜻할 수 있습니다. 구간 다이내믹은 3초 창의 조용한 부분과 큰 부분을 비교합니다.</p><h2>LUFS</h2><p>0에 가까울수록 더 크게 들립니다. Integrated는 전체, Max Momentary는 가장 큰 400 ms, Max Short-term은 가장 큰 3초입니다. 대략 −23 LUFS는 작음, −18은 보통, −14는 큼, −9는 매우 큼입니다. 이 값은 추정치이며 인증된 EBU R128 미터가 아닙니다.</p><h2>스테레오, FFT, 스펙트로그램</h2><p>상관도 +1은 유사한 채널, 0은 넓은 스테레오, 음수는 모노 변환 시 상쇄 가능성을 뜻합니다. FFT는 왼쪽에 저역, 오른쪽에 고역을 표시합니다. 스펙트로그램은 시간·주파수·에너지를 보여 주어 험, 잡음, 고역 손실과 20 Hz 이하 에너지를 찾습니다.</p><h2>DC, BPM, 조성, 무음</h2><p>DC 오프셋은 헤드룸을 줄입니다. BPM은 신뢰도를 포함하며 절반/두 배 템포로 판단할 수 있습니다. 조성은 전조나 특수 조율에서 틀릴 수 있습니다. 긴 앞뒤 무음은 내보내기 범위를 확인하세요.</p><h2>권장 순서</h2><ol><li>먼저 듣기.</li><li>클리핑, True Peak, DC 확인.</li><li>LUFS, RMS, Crest 비교.</li><li>스테레오와 모노 호환 확인.</li><li>파형, FFT, 스펙트로그램으로 문제 위치 확인.</li></ol>)HTML");
    if (language == "zh") return QStringLiteral(R"HTML(<h1>AudioInspector Qt — 如何理解分析结果</h1><p>本应用只分析文件，不会修改音频。单个数字不能决定音质，应把测量结果与实际聆听结合起来。</p><h2>波形</h2><p>横轴是时间，纵轴是振幅。1:1 模式显示类似示波器的真实轨迹；缩小时使用区间真实最小值/最大值，避免混叠。鼓击通常有快速起音，随后是逐渐衰减的振荡。</p><h2>dBFS、采样峰值与 True Peak</h2><p>0 dBFS 是数字上限。−6 dBFS 留有较多余量，−1 dBFS 已接近上限。Sample Peak 是存储采样中的最高值；4× True Peak 用于估计采样之间的峰值。转换时保留约 −1 dBTP 较稳妥。大量帧持续触顶通常表示削波。</p><h2>RMS、峰值因数与动态</h2><p>RMS 表示各声道的平均能量，持续的左右差异可能是声道不平衡。峰值因数是 Peak 与 RMS 的差；数值很小可能表示压缩或限制较重。分段动态比较每个3秒窗口中的安静与响亮部分。</p><h2>LUFS 响度</h2><p>越接近零，主观响度越大。Integrated 表示整首平均值，Max Momentary 是最响的400 ms，Max Short-term 是最响的3秒。粗略参考：−23 LUFS 较轻，−18 中等，−14 较响，−9 非常响。本应用给出估算值，并非经认证的 EBU R128 仪表。</p><h2>立体声、FFT 与频谱图</h2><p>相关度接近 +1 表示左右相似，接近0常表示较宽的立体声，负值可能在转为单声道时发生抵消。FFT 左侧是低频、右侧是高频。频谱图同时显示时间、频率与能量，可用于寻找嗡声、噪声、高频缺失和20 Hz以下的次声能量。</p><h2>DC、BPM、调性与静音</h2><p>直流偏移会减少余量。BPM 带有置信度，也可能识别成一半或两倍速度。调性估计在转调、调式或特殊调音中可能不准确。过长的首尾静音可能表示导出范围不合适。</p><h2>建议步骤</h2><ol><li>先聆听。</li><li>检查削波、True Peak 与 DC。</li><li>比较 LUFS、RMS 与峰值因数。</li><li>一起检查立体声和单声道兼容性。</li><li>用波形、FFT 和频谱图定位问题。</li></ol>)HTML");
    if (language == "ca") return QStringLiteral(R"HTML(
<h1>AudioInspector Qt — Com entendre l’anàlisi</h1><p>El programa estudia un fitxer cada vegada i no el modifica. Cada mesura descriu una part diferent del so; cap xifra, tota sola, determina la qualitat.</p>
<h2>Forma d’ona</h2><p>El temps avança d’esquerra a dreta i l’amplitud és vertical. <b>Mostres 1:1</b> mostra el traç d’oscil·loscopi; en allunyar-se s’utilitza l’envolupant mínim/màxim per evitar aliasing. Un cop de bateria sol tenir un atac ràpid i una oscil·lació que s’apaga.</p>
<h2>Nivell: dBFS, pic i true peak</h2><p>0 dBFS és el sostre digital. −6 dBFS deixa marge i −1 dBFS és molt a prop del límit. El pic de mostra és el valor emmagatzemat més alt. El <b>true peak</b> 4× estima pics entre mostres; convé deixar aproximadament −1 dBTP de marge per a conversions.</p>
<h2>RMS, factor de cresta i dinàmica</h2><p>RMS descriu l’energia mitjana de cada canal. Una diferència persistent entre esquerra i dreta pot indicar desequilibri. El factor de cresta és la diferència entre pic i RMS: un valor baix acostuma a indicar més compressió. La dinàmica per seccions compara finestres de tres segons.</p>
<h2>Sonoritat LUFS</h2><p>Com més a prop de zero, més fort. La LUFS integrada resumeix tot el fitxer; la momentània màxima usa 400 ms i la short-term màxima 3 s. −23 LUFS és suau, −18 moderat, −14 força fort i −9 molt fort, només com a orientació. Són estimacions, no mesures EBU R128 certificades.</p>
<h2>Estèreo i mono</h2><p>Correlació prop de +1 significa canals semblants; prop de 0, estèreo ample; sota 0, possible cancel·lació en mono. Cal llegir-la juntament amb el canvi de nivell mono.</p>
<h2>FFT i espectrograma</h2><p>La FFT situa greus a l’esquerra i aguts a la dreta. L’espectrograma mostra temps, freqüència i energia amb color. Serveixen per localitzar brunzits, sorolls, manca d’aguts o excés subsònic, no per buscar una corba perfecta.</p>
<h2>DC, clipping, BPM i tonalitat</h2><p>Una component DC desplaça el senyal i redueix marge. Els frames retallats toquen el límit digital. El BPM és una estimació amb confiança i pot confondre tempo, meitat o doble tempo. La tonalitat és orientativa i pot fallar amb canvis, modes o afinacions especials.</p>
<h2>Mètode recomanat</h2><ol><li>Escolta primer.</li><li>Revisa clipping, true peak i DC.</li><li>Compara LUFS, RMS i cresta.</li><li>Comprova estèreo i mono.</li><li>Localitza problemes amb forma d’ona, FFT i espectrograma.</li></ol><p>Les mesures ajuden a investigar; l’escolta crítica decideix.</p>)HTML");
    if (language == "gl") return QStringLiteral(R"HTML(
<h1>AudioInspector Qt — Como entender a análise</h1><p>O programa examina un ficheiro cada vez e non o modifica. Cada medida describe unha propiedade diferente; unha cifra illada non determina a calidade.</p>
<h2>Forma de onda</h2><p>O tempo vai de esquerda a dereita e a amplitude é vertical. <b>Mostras 1:1</b> amosa o trazo de osciloscopio; ao afastar a vista emprégase a envolvente mínimo/máximo para evitar aliasing. Un golpe de batería adoita ter un ataque rápido seguido dunha oscilación decrecente.</p>
<h2>dBFS, pico e true peak</h2><p>0 dBFS é o teito dixital. −6 dBFS deixa marxe e −1 dBFS está preto do límite. O pico de mostra é o valor almacenado máis alto. O <b>true peak</b> 4× estima picos entre mostras; arredor de −1 dBTP ofrece marxe útil para conversións.</p>
<h2>RMS, factor de crista e dinámica</h2><p>RMS describe a enerxía media de cada canle. Unha diferenza persistente entre esquerda e dereita pode indicar desequilibrio. O factor de crista é pico menos RMS: valores baixos adoitan indicar máis compresión. A dinámica por seccións compara xanelas de tres segundos.</p>
<h2>Sonoridade LUFS</h2><p>Canto máis preto de cero, máis forte. A integrada resume todo o ficheiro; a momentánea máxima usa 400 ms e a short-term máxima 3 s. −23 LUFS é suave, −18 moderado, −14 forte e −9 moi forte como orientación. Son estimacións, non medicións EBU R128 certificadas.</p>
<h2>Estéreo e mono</h2><p>Correlación preto de +1 significa canles semellantes; preto de 0, estéreo amplo; por baixo de 0, posible cancelación en mono. Debe interpretarse xunto coa variación de nivel mono.</p>
<h2>FFT e espectrograma</h2><p>A FFT sitúa graves á esquerda e agudos á dereita. O espectrograma combina tempo, frecuencia e enerxía mediante cor. Axudan a localizar zunidos, ruídos, perda de agudos ou exceso subsónico; non existe unha curva perfecta para todos os estilos.</p>
<h2>DC, clipping, BPM e tonalidade</h2><p>A compoñente DC despraza o sinal e resta marxe. Os frames recortados alcanzan o límite dixital. BPM inclúe confianza e pode confundir tempo, metade e dobre tempo. A tonalidade é orientativa e pode fallar con cambios, modos ou afinacións especiais.</p>
<h2>Procedemento recomendado</h2><ol><li>Escoita primeiro.</li><li>Revisa clipping, true peak e DC.</li><li>Compara LUFS, RMS e crista.</li><li>Comproba estéreo e mono.</li><li>Localiza problemas coa onda, FFT e espectrograma.</li></ol><p>As medidas axudan a investigar; a escoita crítica decide.</p>)HTML");
    if (language == "eu") return QStringLiteral(R"HTML(
<h1>AudioInspector Qt — Azterketa ulertzeko gida</h1><p>Programak fitxategi bakarra aztertzen du aldi bakoitzean, eta ez du aldatzen. Neurri bakoitzak soinuaren alderdi bat azaltzen du; zenbaki bakar batek ez du kalitatea erabakitzen.</p>
<h2>Uhin-forma</h2><p>Denbora ezkerretik eskuinera doa, eta anplitudea bertikala da. <b>1:1 laginak</b> osziloskopioaren trazua erakusten du; urruntzean minimo/maximo inguratzailea erabiltzen da aliasinga saihesteko. Bateria-kolpe batek eraso azkarra eta pixkanaka itzaltzen den oszilazioa izaten ditu.</p>
<h2>dBFS, lagin-gailurra eta true peak</h2><p>0 dBFS sabaia digitala da. −6 dBFS-k tartea uzten du; −1 dBFS mugatik oso hurbil dago. Lagin-gailurra gordetako baliorik handiena da. 4× <b>true peak</b> neurketak laginen arteko gailurrak estimatzen ditu; −1 dBTP inguruko tartea erabilgarria da bihurketetarako.</p>
<h2>RMS, gandor-faktorea eta dinamika</h2><p>RMS-k kanal bakoitzaren batez besteko energia azaltzen du. Ezker eta eskuinaren arteko alde iraunkorrak desoreka adieraz dezake. Gandor-faktorea gailurraren eta RMS-ren arteko aldea da: balio txikiek konpresio handiagoa adierazi ohi dute. Sekzio-dinamikak hiru segundoko leihoak alderatzen ditu.</p>
<h2>LUFS ozentasuna</h2><p>Zenbat eta zerotik hurbilago, orduan eta ozenago. Integratuak fitxategi osoa laburbiltzen du; momentuko maximoak 400 ms erabiltzen ditu, eta short-term maximoak 3 s. Orientazio gisa: −23 LUFS leuna da, −18 ertaina, −14 ozena eta −9 oso ozena. Estimazioak dira, ez EBU R128 neurketa ziurtatuak.</p>
<h2>Estereoa eta mono bateragarritasuna</h2><p>+1 inguruko korrelazioak antzeko kanalak adierazten ditu; 0 ingurukoak estereo zabala; zero azpikoak mono bihurtzean ezeztapen-arriskua. Mono mailaren aldaketarekin batera irakurri behar da.</p>
<h2>FFT eta espektrograma</h2><p>FFT-k baxuak ezkerrean eta altuak eskuinean erakusten ditu. Espektrogramak denbora, maiztasuna eta energia kolorez uztartzen ditu. Burrunbak, zaratak, altuen galera edo energia subsonikoa aurkitzeko balio dute; ez dago estilo guztientzako kurba perfekturik.</p>
<h2>DC, clipping, BPM eta tonalitatea</h2><p>DC osagaiak seinalea desplazatu eta tartea murrizten du. Moztutako frameek muga digitala ukitzen dute. BPM-k konfiantza dauka eta tempoa erdi edo bikoitzarekin nahas dezake. Tonalitatea orientagarria da eta aldaketekin, moduekin edo afinazio bereziekin huts egin dezake.</p>
<h2>Gomendatutako lana</h2><ol><li>Lehenik entzun.</li><li>Begiratu clipping-a, true peak-a eta DC-a.</li><li>Alderatu LUFS, RMS eta gandorra.</li><li>Egiaztatu estereoa eta monoa.</li><li>Aurkitu arazoak uhin-formarekin, FFTrekin eta espektrogramarekin.</li></ol><p>Neurriek ikertzen laguntzen dute; entzumen kritikoak erabakitzen du.</p>)HTML");
    if (language == "es") return QStringLiteral(R"HTML(
<h1>AudioInspector Qt — Cómo entender el análisis</h1>
<p>Esta ayuda está pensada para poder leer los resultados sin ser ingeniero de sonido. AudioInspector examina un archivo cada vez y <b>no modifica su contenido</b>. Las medidas describen aspectos diferentes: volumen, dinámica, estéreo, frecuencias y posibles defectos. Una cifra aislada rara vez significa que el audio sea bueno o malo.</p>

<h2>1. Forma de onda: una fotografía del sonido</h2>
<p>La forma de onda representa el tiempo de izquierda a derecha y la amplitud en vertical. La línea central equivale al silencio; cuanto más se aleja el trazo de ella, mayor es la amplitud instantánea.</p>
<p><b>Muestras 1:1</b> enseña una traza de osciloscopio para observar ciclos, transitorios y recortes. Al alejarse no caben todas las muestras en pantalla, por lo que se presenta una <b>envolvente mínimo/máximo</b>: conserva los extremos reales de cada tramo sin inventar oscilaciones por aliasing.</p>
<p>Un golpe de batería suele comenzar con un ataque alto y muy rápido, seguido de una oscilación que va disminuyendo. Una parte constantemente pegada a +1 y −1 puede indicar compresión extrema o clipping. La línea naranja señala la posición de reproducción.</p>

<h2>2. dBFS y pico de muestra</h2>
<p><b>dBFS</b> significa decibelios respecto al máximo digital. La escala termina en <b>0 dBFS</b>; no existen muestras PCM válidas por encima. Por eso los valores normales son negativos.</p>
<ul><li><b>−12 dBFS:</b> bastante margen.</li><li><b>−6 dBFS:</b> margen cómodo.</li><li><b>−1 dBFS:</b> muy próximo al límite.</li><li><b>0 dBFS:</b> se ha alcanzado el techo digital.</li></ul>
<p><b>Pico de muestra</b> es la muestra individual más alta del archivo. Llegar una vez a 0 dBFS no demuestra por sí solo que se oiga distorsión, pero muchas muestras consecutivas al límite forman una zona recortada y sí son una señal clara de clipping.</p>

<h2>3. True peak o pico entre muestras</h2>
<p>Dos muestras pueden estar por debajo de cero y, aun así, la curva reconstruida entre ellas superar el límite. Eso puede producir saturación al convertir a analógico, cambiar de formato o aplicar compresión con pérdida.</p>
<p><b>True peak</b> intenta encontrar esos picos intermedios. AudioInspector utiliza una <b>estimación con sobremuestreo 4×</b>; es útil como advertencia, aunque no sustituye a un medidor certificado. Como orientación, dejar el true peak por debajo de −1 dBTP ofrece margen razonable para muchas conversiones.</p>

<h2>4. RMS: energía media de cada canal</h2>
<p>El pico indica el instante más alto; el <b>RMS</b> describe mejor la energía media. Dos canciones pueden alcanzar el mismo pico y sonar muy distintas si una mantiene mucha más energía durante todo el tiempo.</p>
<p>Se muestran RMS izquierdo y derecho. Una diferencia pequeña puede formar parte de la mezcla. Una diferencia mantenida superior a aproximadamente 1,5 dB merece revisión porque puede indicar desequilibrio, un canal defectuoso o una grabación deliberadamente desplazada.</p>

<h2>5. Factor de cresta</h2>
<p>Es la diferencia entre el pico y el RMS. Explica cuánto sobresalen los transitorios respecto al nivel medio.</p>
<ul><li><b>Valor alto:</b> ataques claros y bastante espacio dinámico.</li><li><b>Valor medio:</b> comportamiento habitual en muchas mezclas.</li><li><b>Valor muy bajo:</b> señal densa, limitada o fuertemente comprimida.</li></ul>
<p>No existe un valor ideal universal: música clásica, jazz, electrónica y metal suelen tener perfiles muy distintos.</p>

<h2>6. Sonoridad LUFS</h2>
<p>LUFS intenta aproximarse a cómo percibe el oído el volumen, por eso resulta más útil que el pico para comparar canciones. <b>Cuanto más cerca de cero, mayor sonoridad.</b></p>
<ul><li><b>LUFS integrada:</b> promedio representativo de todo el archivo; es la cifra principal para comparar temas.</li><li><b>Momentánea máxima:</b> ventana más intensa de 400 ms; reacciona a fragmentos breves.</li><li><b>Short-term máxima:</b> ventana más intensa de 3 segundos; representa una sección fuerte sostenida.</li></ul>
<p>Como referencia aproximada, −23 LUFS es suave, −18 LUFS moderado, −14 LUFS relativamente fuerte y −9 LUFS muy fuerte y normalmente bastante comprimido. No son objetivos obligatorios. AudioInspector muestra <b>estimaciones</b> y no un resultado certificado EBU R128.</p>

<h2>7. Dinámica por secciones</h2>
<p>El archivo se divide en ventanas de tres segundos. Se comparan una sección tranquila, la mediana y una sección fuerte. El rango mostrado indica cuánto cambia la energía a lo largo de la pieza.</p>
<p>Un rango pequeño puede corresponder a una canción deliberadamente constante o a una masterización muy comprimida. Un rango grande puede ser deseable en música expresiva, pero también dificultar la escucha en coche o en ambientes ruidosos.</p>

<h2>8. Correlación estéreo y compatibilidad mono</h2>
<p><b>Correlación</b> compara la relación de fase entre los canales:</p>
<ul><li><b>Cerca de +1:</b> canales muy parecidos; imagen estrecha y buena compatibilidad mono.</li><li><b>Cerca de 0:</b> estéreo amplio o canales con información diferente.</li><li><b>Por debajo de 0:</b> partes fuera de fase; al convertir a mono pueden desaparecer elementos.</li></ul>
<p><b>Cambio mono</b> estima cuánto cae el nivel al sumar L y R. Una pérdida notable junto con correlación baja es una advertencia más convincente que cualquiera de las dos medidas por separado.</p>

<h2>9. FFT y balance espectral</h2>
<p>La <b>FFT</b> descompone el audio por frecuencias. La izquierda representa graves y la derecha agudos; el eje es logarítmico para parecerse a la percepción musical. Una elevación no es automáticamente un defecto: el resultado depende de instrumentos, mezcla y estilo.</p>
<ul><li><b>Subsónico, menos de 20 Hz:</b> normalmente no se oye, pero consume margen y puede forzar altavoces.</li><li><b>Graves, 20–250 Hz:</b> bombo, bajo y cuerpo.</li><li><b>Medios, 250 Hz–4 kHz:</b> gran parte de voces e instrumentos.</li><li><b>Agudos, más de 4 kHz:</b> brillo, ataque y aire.</li></ul>
<p>Conviene comparar el balance con una referencia del mismo género, no con una curva supuestamente perfecta.</p>

<h2>10. Espectrograma</h2>
<p>El espectrograma añade el tiempo a la frecuencia: izquierda a derecha es tiempo, abajo hacia arriba es frecuencia y el color indica intensidad. Permite encontrar zumbidos continuos, silbidos, pérdida repentina de agudos, ruidos cortos y cambios entre secciones.</p>
<p>Los colores son relativos al propio archivo. Sirven para localizar estructuras, no para comparar directamente el color de dos archivos analizados por separado.</p>

<h2>11. Componente DC</h2>
<p>Una señal debería oscilar alrededor de cero. La <b>componente DC</b> indica que está desplazada hacia arriba o abajo. Un desplazamiento apreciable roba margen, puede provocar clics al editar y suele proceder de equipos o procesos defectuosos. Valores diminutos son normales por redondeo.</p>

<h2>12. Clipping</h2>
<p>Los <b>frames recortados</b> cuentan posiciones donde uno de los canales toca prácticamente el límite digital. Unas pocas coincidencias pueden proceder de la masterización; una gran cantidad o una meseta visible en la onda exige atención. True peak y clipping son problemas relacionados, pero no idénticos.</p>

<h2>13. BPM y porcentaje de confianza</h2>
<p>BPM significa pulsos por minuto. El programa busca repeticiones de energía y propone el tempo más probable. La <b>confianza</b> expresa cuánto destaca ese candidato frente a otras posibilidades.</p>
<p>Una confianza alta suele aparecer con percusión regular. Una confianza baja es normal en música ambiental, clásica, ritmos variables o introducciones largas. También puede confundirse entre tempo y mitad/doble tempo: 60, 120 y 240 BPM pueden describir musicalmente el mismo pulso a escalas diferentes.</p>

<h2>14. Tonalidad musical</h2>
<p>La tonalidad estimada combina la energía de las doce notas y la compara con perfiles mayor y menor. Es una orientación útil para clasificación o mezclas, pero puede fallar en canciones modales, atonales, con cambios de tonalidad o con afinaciones no estándar.</p>

<h2>15. Silencio inicial y final</h2>
<p>Indica cuánto tarda en comenzar el contenido y cuánto silencio queda después de terminar. Un pequeño margen puede ser intencionado. Varios segundos pueden resultar molestos en reproducción continua o revelar una exportación mal recortada.</p>

<h2>Cómo evaluar un archivo</h2>
<ol><li>Escúchalo primero.</li><li>Comprueba clipping, true peak y componente DC.</li><li>Compara LUFS, RMS y factor de cresta para comprender nivel y compresión.</li><li>Revisa correlación y cambio mono juntos.</li><li>Usa forma de onda, FFT y espectrograma para localizar el problema.</li><li>Compara siempre con material de estilo y procedencia parecidos.</li></ol>
<p><b>Importante:</b> ninguna medida aislada determina la calidad artística ni técnica. Las cifras ayudan a investigar; la escucha crítica toma la decisión final.</p>)HTML");

    return QStringLiteral(R"HTML(
<h1>AudioInspector Qt — Understanding the analysis</h1>
<p>This guide explains the results without assuming an audio-engineering background. AudioInspector examines one file at a time and <b>never modifies it</b>. Each measurement describes a different property, so no single number decides whether a recording is good or bad.</p>
<h2>1. Waveform</h2><p>Time runs from left to right and amplitude is vertical. The centre line is silence. <b>1:1 samples</b> displays an oscilloscope trace; zoomed-out views use the real minimum and maximum of every interval to avoid aliasing. A drum hit normally has a fast attack followed by a decaying oscillation.</p>
<h2>2. Sample peak and dBFS</h2><p>0 dBFS is the digital ceiling, so ordinary values are negative. −6 dBFS leaves comfortable headroom, while −1 dBFS is close to the limit. A sample touching zero is not automatically audible distortion, but many consecutive clipped samples are a clear warning.</p>
<h2>3. True peak</h2><p>The reconstructed curve may exceed the ceiling between stored samples. The 4× true-peak estimate looks for these inter-sample overs. Keeping below roughly −1 dBTP provides useful conversion headroom. This is an estimate, not a certified meter.</p>
<h2>4. RMS and crest factor</h2><p><b>RMS</b> describes average energy in each channel. A persistent L/R difference may reveal imbalance. <b>Crest factor</b> is peak minus RMS: higher values generally retain stronger transients; very low values often indicate dense limiting or compression. Musical style matters greatly.</p>
<h2>5. LUFS loudness</h2><p>LUFS approximates perceived loudness. Values nearer zero are louder. <b>Integrated</b> describes the whole file, <b>max momentary</b> the loudest 400 ms and <b>max short-term</b> the loudest 3 seconds. Roughly, −23 LUFS is quiet, −18 moderate, −14 fairly loud and −9 very loud. Results are estimates rather than certified EBU R128 measurements.</p>
<h2>6. Section dynamics</h2><p>Three-second windows are compared to show quiet, median and loud sections. A small range can be intentional or indicate heavy compression; a large range can sound expressive but may be difficult in noisy environments.</p>
<h2>7. Stereo and mono compatibility</h2><p>Correlation near +1 means similar channels; near 0 often means wider stereo; negative values warn of phase cancellation in mono. <b>Mono change</b> estimates the level loss after summing L and R. Read both values together.</p>
<h2>8. FFT and spectral balance</h2><p>The FFT separates frequencies on a logarithmic axis: bass on the left and treble on the right. Subsonic energy below 20 Hz is usually inaudible but can waste headroom. Lows cover approximately 20–250 Hz, mids 250 Hz–4 kHz and highs above 4 kHz. Compare against similar material rather than an imaginary perfect curve.</p>
<h2>9. Spectrogram</h2><p>Time is horizontal, frequency vertical and colour shows relative energy. It helps locate hum, whistles, short noises, missing high frequencies and section changes. Colours are scaled to each file and should not be compared directly between separate analyses.</p>
<h2>10. DC offset and clipping</h2><p>Audio should oscillate around zero. Significant DC offset wastes headroom and can create clicks during editing. Clipped frames count positions touching the digital limit. Clipping and true peak are related but different problems.</p>
<h2>11. BPM and confidence</h2><p>BPM estimates tempo from repeating energy. Confidence shows how strongly that candidate stands out. Ambient, classical or changing rhythms may be uncertain, and half/double-tempo ambiguity is common.</p>
<h2>12. Musical key</h2><p>The estimate compares the twelve pitch classes with major and minor profiles. It is guidance only and may fail with key changes, modes, atonal music or non-standard tuning.</p>
<h2>13. Leading and trailing silence</h2><p>These values report quiet time before and after the content. A short margin may be intentional; several seconds can disrupt continuous playback or indicate an imprecise export.</p>
<h2>A practical workflow</h2><ol><li>Listen first.</li><li>Check clipping, true peak and DC.</li><li>Compare LUFS, RMS and crest factor.</li><li>Read correlation and mono change together.</li><li>Use waveform, FFT and spectrogram to locate issues.</li><li>Compare with recordings of a similar style and source.</li></ol>
<p><b>Remember:</b> measurements support investigation; critical listening makes the final decision.</p>)HTML");
}

QString principlesHtml(const QString &language)
{
    // Manifiesto del proyecto. Se muestra solo en «Acerca de», no en la ayuda,
    // para evitar duplicación y separar filosofía de instrucciones de uso.
    if (language == "es") return QStringLiteral(R"HTML(<hr><h2>Principios del proyecto</h2><p><b>AudioInspector Qt nace como respuesta a la desinformación técnica en el mundo del audio.</b> Sustituye términos ambiguos y afirmaciones imposibles de comprobar por medidas reproducibles, gráficos verificables y métodos documentados.</p><p>No pretende decidir qué debe gustarte ni presentar una preferencia subjetiva como una verdad física. Su objetivo es mostrar qué contiene realmente el archivo digital, distinguir mediciones de estimaciones y declarar claramente sus limitaciones.</p><blockquote>Si una afirmación técnica es cierta, debe poder definirse, medirse y repetirse.</blockquote><p>El análisis comienza en la fuente digital. El DAC, la amplificación, los transductores, la sala y la percepción humana pertenecen a etapas posteriores y requieren sus propias mediciones. AudioInspector no atribuye al archivo propiedades que no puede demostrar.</p>)HTML");
    if (language == "ca") return QStringLiteral(R"HTML(<hr><h2>Principis del projecte</h2><p><b>AudioInspector Qt neix com a resposta a la desinformació tècnica en el món de l’àudio.</b> Substitueix termes ambigus per mesures reproduïbles, gràfics verificables i mètodes documentats.</p><p>No decideix què t’ha d’agradar ni presenta una preferència com un fet físic. Mostra què conté el fitxer, separa mesures d’estimacions i declara els límits.</p><blockquote>Si una afirmació tècnica és certa, s’ha de poder definir, mesurar i repetir.</blockquote>)HTML");
    if (language == "gl") return QStringLiteral(R"HTML(<hr><h2>Principios do proxecto</h2><p><b>AudioInspector Qt nace como resposta á desinformación técnica no mundo do audio.</b> Substitúe termos ambiguos por medidas reproducibles, gráficos verificables e métodos documentados.</p><p>Non decide o que debe gustar nin presenta unha preferencia como feito físico. Amosa o contido do ficheiro, separa medicións de estimacións e declara os límites.</p><blockquote>Se unha afirmación técnica é certa, debe poder definirse, medirse e repetirse.</blockquote>)HTML");
    if (language == "eu") return QStringLiteral(R"HTML(<hr><h2>Proiektuaren printzipioak</h2><p><b>AudioInspector Qt audioaren munduko desinformazio teknikoari erantzuteko sortu da.</b> Termino lausoen ordez neurri errepikagarriak, grafiko egiaztagarriak eta dokumentatutako metodoak erabiltzen ditu.</p><p>Ez du zer gustatu behar zaizun erabakitzen, eta ez du lehentasun subjektiborik egitate fisiko gisa aurkezten.</p><blockquote>Baieztapen tekniko bat egia bada, definitu, neurtu eta errepikatu ahal izan behar da.</blockquote>)HTML");
    if (language == "fr") return QStringLiteral(R"HTML(<hr><h2>Principes du projet</h2><p><b>AudioInspector Qt répond à la désinformation technique dans le monde de l’audio.</b> Il remplace les termes ambigus par des mesures reproductibles, des graphiques vérifiables et des méthodes documentées.</p><p>Il ne décide pas de vos goûts et ne transforme pas une préférence en fait physique. Il sépare mesures, estimations et limites.</p><blockquote>Une affirmation technique vraie doit pouvoir être définie, mesurée et reproduite.</blockquote>)HTML");
    if (language == "de") return QStringLiteral(R"HTML(<hr><h2>Projektgrundsätze</h2><p><b>AudioInspector Qt ist eine Antwort auf technische Fehlinformation in der Audiowelt.</b> Mehrdeutige Begriffe werden durch reproduzierbare Messungen, überprüfbare Grafiken und dokumentierte Methoden ersetzt.</p><p>Die Anwendung entscheidet nicht über Geschmack und erklärt keine Vorliebe zur physikalischen Tatsache.</p><blockquote>Eine wahre technische Behauptung muss definierbar, messbar und wiederholbar sein.</blockquote>)HTML");
    if (language == "it") return QStringLiteral(R"HTML(<hr><h2>Principi del progetto</h2><p><b>AudioInspector Qt nasce come risposta alla disinformazione tecnica nel mondo dell’audio.</b> Sostituisce termini ambigui con misure riproducibili, grafici verificabili e metodi documentati.</p><p>Non decide cosa debba piacere e non trasforma una preferenza in un fatto fisico.</p><blockquote>Un’affermazione tecnica vera deve poter essere definita, misurata e ripetuta.</blockquote>)HTML");
    if (language == "pt") return QStringLiteral(R"HTML(<hr><h2>Princípios do projeto</h2><p><b>AudioInspector Qt nasce como resposta à desinformação técnica no mundo do áudio.</b> Substitui termos ambíguos por medições reproduzíveis, gráficos verificáveis e métodos documentados.</p><p>Não decide preferências nem transforma gosto subjetivo em facto físico.</p><blockquote>Uma afirmação técnica verdadeira deve poder ser definida, medida e repetida.</blockquote>)HTML");
    if (language == "ja") return QStringLiteral(R"HTML(<hr><h2>プロジェクトの原則</h2><p><b>AudioInspector Qt は、オーディオ分野の技術的な誤情報に対し、再現可能な測定、検証できるグラフ、文書化された方法を提示します。</b></p><p>好みを決めたり、主観を物理的事実として扱ったりしません。測定、推定、限界を明確に分けます。</p><blockquote>正しい技術的主張は、定義・測定・再現できなければなりません。</blockquote>)HTML");
    if (language == "ko") return QStringLiteral(R"HTML(<hr><h2>프로젝트 원칙</h2><p><b>AudioInspector Qt는 오디오 분야의 기술적 오해에 맞서 재현 가능한 측정, 검증 가능한 그래프, 문서화된 방법을 제공합니다.</b></p><p>취향을 결정하거나 주관을 물리적 사실로 제시하지 않으며 측정, 추정, 한계를 구분합니다.</p><blockquote>참인 기술적 주장은 정의하고 측정하며 반복할 수 있어야 합니다.</blockquote>)HTML");
    if (language == "zh") return QStringLiteral(R"HTML(<hr><h2>项目原则</h2><p><b>AudioInspector Qt 以可重复的测量、可验证的图表和有记录的方法，对抗音频领域的技术误导。</b></p><p>它不决定个人喜好，也不把主观偏好包装成物理事实，并明确区分测量、估算与局限。</p><blockquote>真实的技术主张必须能够被定义、测量和重复验证。</blockquote>)HTML");
    return QStringLiteral(R"HTML(<hr><h2>Project principles</h2><p><b>AudioInspector Qt is a response to technical misinformation in audio.</b> It replaces ambiguous language and untestable claims with reproducible measurements, verifiable graphs and documented methods.</p><p>It does not decide what you should enjoy or present preference as physical fact. It separates measurements, estimates and limitations.</p><blockquote>If a technical claim is true, it must be possible to define, measure and reproduce it.</blockquote><p>Analysis begins at the digital source. DACs, amplification, transducers, rooms and perception are later stages requiring their own measurements.</p>)HTML");
}

QString legalHtml(const QString &language)
{
    // El significado legal permanece idéntico en todas las traducciones: GPLv3
    // para el código, ausencia de garantía e identidad visual fuera de la GPL.
    const QString copyright = QStringLiteral("<p><b>Copyright © 2026 Tomás Fernández Galera.</b></p>");
    if (language == "es") return copyright + QStringLiteral("<p>El código fuente se distribuye bajo la Licencia Pública General GNU versión 3. Este programa se proporciona sin garantía. El nombre AudioInspector Qt, su icono y su identidad visual no se conceden bajo la GPL; consulta TRADEMARKS.md.</p>");
    if (language == "ca") return copyright + QStringLiteral("<p>El codi font es distribueix sota la Llicència Pública General GNU versió 3. Aquest programa es proporciona sense garantia. El nom AudioInspector Qt, la icona i la identitat visual no es concedeixen sota la GPL; consulteu TRADEMARKS.md.</p>");
    if (language == "gl") return copyright + QStringLiteral("<p>O código fonte distribúese baixo a Licenza Pública Xeral GNU versión 3. Este programa proporciónase sen garantía. O nome AudioInspector Qt, a icona e a identidade visual non se conceden baixo a GPL; consulta TRADEMARKS.md.</p>");
    if (language == "eu") return copyright + QStringLiteral("<p>Iturburu-kodea GNU Lizentzia Publiko Orokorraren 3. bertsioaren arabera banatzen da. Programa hau bermerik gabe eskaintzen da. AudioInspector Qt izena, ikonoa eta ikusizko identitatea ez dira GPLren bidez ematen; ikus TRADEMARKS.md.</p>");
    if (language == "fr") return copyright + QStringLiteral("<p>Le code source est distribué sous la licence publique générale GNU version 3. Ce programme est fourni sans garantie. Le nom AudioInspector Qt, son icône et son identité visuelle ne sont pas concédés sous GPL ; voir TRADEMARKS.md.</p>");
    if (language == "de") return copyright + QStringLiteral("<p>Der Quellcode wird unter der GNU General Public License Version 3 verbreitet. Dieses Programm wird ohne Gewährleistung bereitgestellt. Name, Symbol und visuelle Identität von AudioInspector Qt werden nicht unter der GPL lizenziert; siehe TRADEMARKS.md.</p>");
    if (language == "it") return copyright + QStringLiteral("<p>Il codice sorgente è distribuito secondo la GNU General Public License versione 3. Il programma è fornito senza garanzia. Il nome AudioInspector Qt, l’icona e l’identità visiva non sono concessi sotto GPL; vedere TRADEMARKS.md.</p>");
    if (language == "pt") return copyright + QStringLiteral("<p>O código-fonte é distribuído sob a Licença Pública Geral GNU versão 3. Este programa é fornecido sem garantia. O nome AudioInspector Qt, o ícone e a identidade visual não são licenciados sob a GPL; consulte TRADEMARKS.md.</p>");
    if (language == "ja") return copyright + QStringLiteral("<p>ソースコードは GNU General Public License バージョン3で配布されます。本プログラムに保証はありません。AudioInspector Qt の名称、アイコン、ビジュアル・アイデンティティは GPL の対象外です。TRADEMARKS.md を参照してください。</p>");
    if (language == "ko") return copyright + QStringLiteral("<p>소스 코드는 GNU General Public License 버전 3에 따라 배포됩니다. 이 프로그램은 보증 없이 제공됩니다. AudioInspector Qt 이름, 아이콘 및 시각적 정체성은 GPL로 허가되지 않습니다. TRADEMARKS.md를 참조하세요.</p>");
    if (language == "zh") return copyright + QStringLiteral("<p>源代码依据 GNU 通用公共许可证第3版发布。本程序不提供任何担保。AudioInspector Qt 的名称、图标和视觉识别不在 GPL 授权范围内；请参阅 TRADEMARKS.md。</p>");
    return copyright + QStringLiteral("<p>The source code is distributed under the GNU General Public License version 3. This program is provided without warranty. The AudioInspector Qt name, icon and visual identity are not licensed under the GPL; see TRADEMARKS.md.</p>");
}
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
