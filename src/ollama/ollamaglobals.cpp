/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <qstringliteral.h>

#include "src/ollama/ollamaglobals.h"

QString OllamaGlobals::PluginName = QStringLiteral("Ollama");
QString OllamaGlobals::IconName   = QStringLiteral("applications-all");
QString OllamaGlobals::HelpText =
    QStringLiteral("Ask a question, press Enter to send.\n(Tip: use CTRL+Enter or SHIFT+Enter for "
                   "adding a new line)");

QString OllamaGlobals::LabelOllamaEndpointOverride = QStringLiteral("Override Ollama endpoint:");
