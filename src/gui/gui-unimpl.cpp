/*
 * Clap Validator Plugin
 *
 * A expanding set of plugins to test your host
 *
 * Copyright 2024-2025,Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license,
 * The source code and license are at https://github.com/free-audio/clap-validator-plugin
 */

#include "gui.h"
#include <clap/clap.h>

namespace free_audio::cvp::gui
{

std::unique_ptr<GuiProvider>
createGuiProvider(std::function<void(clap_log_severity, const std::string &)> logFn)
{
    logFn(CLAP_LOG_WARNING, "GUI Unimplemented");
    return nullptr;
}
} // namespace free_audio::cvp::gui