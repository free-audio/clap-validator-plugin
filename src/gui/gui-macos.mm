//
// Created by Paul Walker on 3/13/25.
//

#include "gui.h"
#include <clap/clap.h>

namespace free_audio::cvp::gui
{

std::unique_ptr<GuiProvider> createGuiProvider(std::function<void(clap_log_severity, const std::string &)> logFn)
{
    logFn(CLAP_LOG_WARNING, "MacOS GUI Unimplemented");
    return nullptr;
}
}