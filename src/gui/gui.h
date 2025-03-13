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

#ifndef FREEAUDIO_CVP_GUI_GUI_H
#define FREEAUDIO_CVP_GUI_GUI_H

#include <memory>
#include <string>
#include <clap/clap.h>
#include <functional>

namespace free_audio::cvp::gui
{
struct GuiProvider
{
    virtual ~GuiProvider() = default;

    virtual bool setParent(const clap_window_t *parent) { return false; }
};

inline bool implementsGui()
{
#if COCOA_GUI
    return true;
#else
    return false;
#endif
}
std::unique_ptr<GuiProvider>
createGuiProvider(std::function<void(clap_log_severity, const std::string &)> logFn);
}; // namespace free_audio::cvp::gui

#endif // GUI_H
