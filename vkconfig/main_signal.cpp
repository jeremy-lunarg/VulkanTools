/*
 * Copyright (c) 2020-2024 Valve Corporation
 * Copyright (c) 2020-2024 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include "../vkconfig_core/override.h"
#include "../vkconfig_core/layer_manager.h"

#include "main_signal.h"
#include "configurator.h"

#include <QSettings>

#include <csignal>

void SurrenderConfiguration(int signal) {
    (void)signal;

    // Indicate that Vulkan Configurator crashed to handle it on next run
    {
        QSettings settings;
        settings.setValue("crashed", true);
    }

    PathManager paths("", SUPPORTED_CONFIG_FILES);
    Environment environment(paths);

    SurrenderConfiguration(environment);
}

void InitSignals() {
    std::signal(SIGINT, SurrenderConfiguration);
    std::signal(SIGTERM, SurrenderConfiguration);
    std::signal(SIGSEGV, SurrenderConfiguration);
    std::signal(SIGABRT, SurrenderConfiguration);
    std::signal(SIGILL, SurrenderConfiguration);
    std::signal(SIGFPE, SurrenderConfiguration);
}
