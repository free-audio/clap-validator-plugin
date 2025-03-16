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
#include <atomic>
#include <clap/clap.h>
#include <functional>
#include <fmt/core.h>

namespace free_audio::cvp
{
struct CVPClap;
}
namespace free_audio::cvp::gui
{
struct GuiProvider
{
    free_audio::cvp::CVPClap *parent{nullptr};
    GuiProvider(CVPClap *par) : parent(par) { logFmt(CLAP_LOG_INFO, "GUI Provider created"); }
    virtual ~GuiProvider() = default;

    template <typename... Args>
    void logFmt(clap_log_severity s, const char *fmt, Args &&...args) const noexcept;

    virtual bool setScale(double scale) noexcept { return false; }
    virtual bool show() noexcept { return false; }

    virtual bool hide() noexcept { return false; }
    static constexpr uint32_t initW{800}, initH{640};
    virtual bool getSize(uint32_t *width, uint32_t *height) noexcept
    {
        *width = initW;
        *height = initH;
        return true;
    }
    virtual bool canResize() const noexcept { return false; }
    virtual bool getResizeHints(clap_gui_resize_hints_t *hints) noexcept { return false; }
    virtual bool adjustSize(uint32_t *width, uint32_t *height) noexcept { return false; }
    virtual bool setSize(uint32_t width, uint32_t height) noexcept { return false; }
    virtual void suggestTitle(const char *title) noexcept {}

    virtual bool setParent(const clap_window_t *parent)
    {
        logFmt(CLAP_LOG_ERROR,
               "setParent called on gui base class - this should never happen. {}:{}", __FILE__,
               __LINE__);
        return false;
    }

    std::string transportString(const clap_event_transport &transport) const
    {
        return fmt::format("beats: {} seconds: {}\nState {} {}",
                           transport.song_pos_beats * 1.0 / CLAP_BEATTIME_FACTOR,
                           transport.song_pos_seconds * 1.0 / CLAP_BEATTIME_FACTOR,
                           transport.flags & CLAP_TRANSPORT_IS_PLAYING ? "Playing" : "Stopped",
                           transport.flags & CLAP_TRANSPORT_IS_RECORDING ? "& Recording" : ""
          );
    }

    std::atomic<bool> logDirty{false};
};

inline bool implementsGui()
{
#if COCOA_GUI
    return true;
#elif WIN32
    return true;
#else
    return false;
#endif
}
std::unique_ptr<GuiProvider> createGuiProvider(CVPClap *);
}; // namespace free_audio::cvp::gui

#endif // GUI_H
