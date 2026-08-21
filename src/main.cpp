#include "mainwindow.h"

#include <QApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QScreen>
#include <QStyleFactory>

#define AUDIOINSPECTOR_STRINGIFY_IMPL(value) #value
#define AUDIOINSPECTOR_STRINGIFY(value) AUDIOINSPECTOR_STRINGIFY_IMPL(value)

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("AudioInspector Qt"));
    app.setOrganizationName(QStringLiteral("Tomas-Fernandez-Galera"));
    app.setApplicationVersion(QString::fromLatin1(
        AUDIOINSPECTOR_STRINGIFY(AUDIOINSPECTOR_VERSION)));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    MainWindow window;
    // Conserva explícitamente el marco estándar de Windows y adapta el tamaño
    // inicial al área útil del monitor. Esto evita que una ventana de 760 px de
    // alto pierda la barra de título en pantallas de 768 px con barra de tareas.
    window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                          | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect available = screen->availableGeometry();
        const QSize safeSize(qMin(1180, available.width() - 32),
                             qMin(760, available.height() - 32));
        window.resize(safeSize);
        window.move(available.center() - QPoint(safeSize.width() / 2,
                                                 safeSize.height() / 2));
    }
    window.show();
    return app.exec();
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
