# AudioInspector Qt

AudioInspector Qt is a measurement-oriented desktop audio analyzer. It replaces ambiguous descriptions and untestable claims with reproducible measurements, verifiable graphs, documented methods and explicit uncertainty.

AudioInspector Qt does not decide what listeners should enjoy and does not present subjective preference as physical fact. Its scope begins with the digital source file: playback hardware, transducers, rooms and human perception are later stages requiring their own measurements.

> If a technical claim is true, it must be possible to define, measure and reproduce it.

The interface and built-in analysis guide are available in English, Spanish, Catalan, Galician, Basque, French, German, Italian, Portuguese, Japanese, Korean and Chinese.

## What it measures

AudioInspector Qt examines the decoded samples and presents the results by technical area:

- **File and signal format:** codec, duration, sample rate, decoded bit depth, channel count and bitrate.
- **Digital level:** sample peak in dBFS, estimated 4× oversampled true peak in dBTP and left/right RMS level.
- **Dynamics:** crest factor, estimated integrated LUFS, maximum momentary and short-term loudness, and dynamic variation between sections.
- **Signal integrity:** clipped frames, DC offset in each channel, digital silence percentage, and leading and trailing silence duration.
- **Stereo behaviour:** left/right balance, stereo correlation and estimated level change when folded down to mono.
- **Spectral distribution:** subsonic, low, mid and high-frequency energy proportions, FFT spectrum and time-frequency spectrogram.
- **Musical estimates:** tempo in BPM with a confidence value and estimated musical key.
- **Visual inspection:** scrollable stereo waveform, true sample-level view at 1:1, full-file peak envelope, playback cursor and time navigation.

The application also generates a copyable technical report with plain-language conclusions. Estimated measurements are identified as estimates, and the report states what cannot be inferred from the digital file alone.

## Current capabilities

- Single-file analysis for WAV, FLAC, MP3, OGG, Opus, M4A, AAC, WMA and AIFF files supported by the installed Qt multimedia backend.
- Stereo waveform with sample-level zoom, full-file envelope, scrolling and playback position.
- Sample peak, estimated true peak, RMS, crest factor, clipping, DC offset and digital silence measurements.
- Estimated LUFS, section dynamics, stereo correlation, mono compatibility, spectral balance, BPM and musical key.
- Spectrogram, FFT graph and a copyable technical report with conclusions and stated limitations.
- Light and dark themes and a localized measurement guide.

## Build on Windows

Requirements:

- Windows 10 or 11.
- Qt 6.5 or later with Qt Multimedia and a Desktop kit.
- Qt Creator, or QMake and MinGW from a configured Qt command prompt.

Open `AudioInspectorQt.pro` in Qt Creator, select the Desktop Qt kit and press **Run**.

## License and identity

Copyright © 2026 Tomás Fernández Galera.

The source code is distributed under the [GNU General Public License version 3](LICENSE). The AudioInspector Qt name, icon and visual identity are not licensed under the GPL; see [TRADEMARKS.md](TRADEMARKS.md).

## Español

AudioInspector Qt es un analizador de audio de escritorio orientado a la medición. Sustituye las descripciones ambiguas y las afirmaciones imposibles de comprobar por mediciones reproducibles, gráficos verificables, métodos documentados e indicaciones explícitas sobre su incertidumbre.

AudioInspector Qt no decide qué debe gustarle al oyente ni presenta una preferencia subjetiva como una verdad física. Su ámbito comienza en el archivo digital de origen: el equipo de reproducción, los transductores, la sala y la percepción humana son etapas posteriores que requieren sus propias mediciones.

> Si una afirmación técnica es cierta, debe ser posible definirla, medirla y reproducirla.

La interfaz y la guía de análisis integrada están disponibles en inglés, español, catalán, gallego, euskera, francés, alemán, italiano, portugués, japonés, coreano y chino.

### Qué mide

AudioInspector Qt examina las muestras decodificadas y presenta los resultados por áreas técnicas:

- **Formato del archivo y de la señal:** códec, duración, frecuencia de muestreo, profundidad de bits decodificada, número de canales y tasa de bits.
- **Nivel digital:** pico de muestra en dBFS, pico verdadero estimado (*true peak*) mediante sobremuestreo 4× en dBTP y nivel RMS de los canales izquierdo y derecho.
- **Dinámica:** factor de cresta, LUFS integrados estimados, sonoridad momentánea máxima y sonoridad máxima a corto plazo, y variación dinámica entre secciones.
- **Integridad de la señal:** tramas recortadas, componente DC de cada canal, porcentaje de silencio digital y duración de los silencios inicial y final.
- **Comportamiento estéreo:** balance entre los canales izquierdo y derecho, correlación estéreo y cambio de nivel estimado al convertir la señal a mono.
- **Distribución espectral:** proporción de energía subsónica, grave, media y aguda, espectro FFT y espectrograma en función del tiempo.
- **Estimaciones musicales:** tempo en BPM acompañado de un porcentaje de confianza y tonalidad musical estimada.
- **Inspección visual:** forma de onda estéreo desplazable, representación real de las muestras en vista 1:1, envolvente de picos del archivo completo, cursor de reproducción y navegación temporal.

La aplicación también genera un informe técnico copiable con conclusiones explicadas en lenguaje sencillo. Las magnitudes aproximadas se identifican como estimaciones y el informe aclara qué aspectos no pueden deducirse únicamente a partir del archivo digital.

### Funciones actuales

- Análisis individual de archivos WAV, FLAC, MP3, OGG, Opus, M4A, AAC, WMA y AIFF admitidos por el sistema multimedia de Qt instalado.
- Forma de onda estéreo con ampliación hasta el nivel de muestra, envolvente del archivo completo, desplazamiento y posición de reproducción.
- Medición de pico de muestra, pico verdadero estimado, RMS, factor de cresta, recorte digital (*clipping*), componente DC y silencio digital.
- Estimaciones de LUFS, dinámica por secciones, correlación estéreo, compatibilidad mono, balance espectral, BPM y tonalidad musical.
- Espectrograma, gráfico FFT e informe técnico copiable con conclusiones y limitaciones declaradas.
- Temas claro y oscuro y guía de mediciones traducida.

### Compilación en Windows

Requisitos:

- Windows 10 u 11.
- Qt 6.5 o posterior con Qt Multimedia y un kit de escritorio.
- Qt Creator, o QMake y MinGW desde una consola configurada para Qt.

Abre `AudioInspectorQt.pro` en Qt Creator, selecciona el kit Qt de escritorio y pulsa **Ejecutar**.

### Licencia e identidad

Copyright © 2026 Tomás Fernández Galera.

El código fuente se distribuye bajo la [Licencia Pública General de GNU, versión 3](LICENSE). El nombre AudioInspector Qt, su icono y su identidad visual no se conceden bajo la GPL; consulta [TRADEMARKS.md](TRADEMARKS.md).
