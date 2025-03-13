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

#include "cvp-plugin-base.h"
namespace free_audio::cvp::gui
{

std::unique_ptr<GuiProvider> createGuiProvider(CVPClap *clap)
{
    clap->logFmt(CLAP_LOG_WARNING, "GUI Unimplemented");
    return nullptr;
}
} // namespace free_audio::cvp::gui