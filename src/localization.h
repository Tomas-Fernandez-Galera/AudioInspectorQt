#pragma once

#include <QString>

namespace Localization {
// Etiquetas cortas usadas por widgets. Si falta una traducción se devuelve el
// texto inglés, de modo que nunca aparezca una clave interna en la interfaz.
QString text(const QString &key, const QString &language);
// Documentos HTML separados: manual técnico, manifiesto y aviso legal tienen
// responsabilidades distintas y pueden mostrarse en ventanas independientes.
QString helpHtml(const QString &language);
QString principlesHtml(const QString &language);
QString legalHtml(const QString &language);
}
// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tomás Fernández Galera
