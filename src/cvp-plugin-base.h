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

#ifndef FREEAUDIO_CVP_CVP_PLUGIN_BASE_H
#define FREEAUDIO_CVP_CVP_PLUGIN_BASE_H

#include <clap/clap.h>
#include <clap/helpers/plugin.hxx>
#include <clap/helpers/host-proxy.hxx>

#include "gui/gui.h"
#include <fmt/core.h>

namespace free_audio::cvp
{
static constexpr clap::helpers::MisbehaviourHandler misLevel =
    clap::helpers::MisbehaviourHandler::Ignore;
static constexpr clap::helpers::CheckingLevel checkLevel = clap::helpers::CheckingLevel::Maximal;

using plugHelper_t = clap::helpers::Plugin<misLevel, checkLevel>;

template <enum ValidatorFlavor flavor> struct CVPClap : public plugHelper_t
{
    CVPClap(const clap_host *h) : plugHelper_t(getDescriptor(flavor), h) {}

  protected:
    // This is not exactly realtime safe
    template <typename... Args>
    void logFmt(clap_log_severity s, const char *fmt, Args &&...args) const noexcept
    {
        auto res = fmt::format(fmt, std::forward<Args>(args)...);
        log(s, res.c_str());
    }

    void logTee(clap_log_severity severity, const char *msg) const noexcept override
    {
        switch (severity)
        {
        case CLAP_LOG_INFO:
            std::cout << "[INFO   ] ";
            break;
        case CLAP_LOG_WARNING:
            std::cout << "[WARNING] ";
            break;
        case CLAP_LOG_ERROR:
            std::cout << "[ERROR  ] ";
            break;
        case CLAP_LOG_DEBUG:
            std::cout << "[DEBUG  ] ";
            break;
        default:
            std::cout << "[UNKNOWN] ";
            return;
        }
        std::cout << msg << std::endl;
    }
    bool implementsAudioPorts() const noexcept override
    {
        switch (flavor)
        {
        case ValidatorFlavor::StereoEffect:
        case ValidatorFlavor::StereoGenerator:
            return true;
        default:
            return false;
        }
    }
    uint32_t audioPortsCount(bool isInput) const noexcept override
    {
        switch (flavor)
        {
        case ValidatorFlavor::StereoEffect:
            return 1; // independent of input
        case ValidatorFlavor::StereoGenerator:
            return isInput ? 0 : 1;
        default:
            break;
        }
        hostMisbehaving("AudioPorts Count should not be called with note only effects");
        return 0;
    }
    bool audioPortsInfo(uint32_t index, bool isInput,
                        clap_audio_port_info *info) const noexcept override
    {
        if (index != 0)
            hostMisbehaving("Invalid audio ports index");
        if (isInput && flavor == ValidatorFlavor::StereoGenerator)
            hostMisbehaving("AudioPorts Info should not be called for input stereo generator");
        info->id = 18472 + index + isInput * 17;
        info->in_place_pair = CLAP_INVALID_ID;
        strncpy(info->name, isInput ? "Main In" : "Main Out", sizeof(info->name));
        info->flags = CLAP_AUDIO_PORT_IS_MAIN;
        info->flags = 0;
        info->channel_count = 2;
        info->port_type = CLAP_PORT_STEREO;
        return true;
    }

    bool implementsNotePorts() const noexcept override
    {
        if (flavor == ValidatorFlavor::StereoEffect)
            return false;
        return true;
    }
    uint32_t notePortsCount(bool isInput) const noexcept override
    {
        if (flavor == ValidatorFlavor::StereoEffect)
            hostMisbehaving("NotePorts Count should not be called with stereo effect");
        switch (flavor)
        {
        case ValidatorFlavor::StereoEffect:
            return 0;
        case ValidatorFlavor::StereoGenerator:
            return isInput ? 1 : 0;
        case ValidatorFlavor::NoteGenerator:
            return isInput ? 0 : 1;
        case ValidatorFlavor::NoteTransformer:
            return 1;
        }
        return 0;
    }
    bool notePortsInfo(uint32_t index, bool isInput,
                       clap_note_port_info *info) const noexcept override
    {
        if (flavor == ValidatorFlavor::StereoEffect)
            hostMisbehaving("NotePorts Info shoould not be called with stereo effect");
        if (isInput && flavor == ValidatorFlavor::NoteGenerator)
            hostMisbehaving("NotePorts Info should not be called for input note generator");
        if (!isInput && flavor == ValidatorFlavor::StereoGenerator)
            hostMisbehaving("NotePorts Info should not be called for output stereo generator");

        info->id = isInput ? 17252 : 87422;
        info->supported_dialects =
            CLAP_NOTE_DIALECT_MIDI | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_CLAP;
        info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        strncpy(info->name, isInput ? "Note Input" : "Note Output", CLAP_NAME_SIZE - 1);
        return true;
    }

    bool implementsGui() const noexcept override { return gui::implementsGui(); }
    bool guiIsApiSupported(const char *api, bool isFloating) noexcept override
    {
        if (isFloating)
            return false;

#if COCOA_GUI
        return strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
#endif

        log(CLAP_LOG_INFO, "Implement gui outside macos");
        return false;
    }

    std::unique_ptr<gui::GuiProvider> guiProvider;
    bool guiCreate(const char *api, bool isFloating) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "{} api={} isFloating={}", __func__, api,
               isFloating ? "floating" : "non-floating");
        if (!guiIsApiSupported(api, isFloating))
        {
            hostMisbehaving("Gui API not supported");
            return false;
        }
        guiProvider = gui::createGuiProvider([this](auto a, auto b) { log(a, b.c_str()); });
        return guiProvider != nullptr;
    }
    void guiDestroy() noexcept override { guiProvider.reset(); }
    bool guiSetScale(double scale) noexcept override { return false; }
    bool guiShow() noexcept override { return false; }

    bool guiHide() noexcept override { return false; }
    bool guiGetSize(uint32_t *width, uint32_t *height) noexcept override
    {
        *width = 800;
        *height = 640;
        return true;
    }
    bool guiCanResize() const noexcept override { return false; }
    // bool guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept override;
    // bool guiAdjustSize(uint32_t *width, uint32_t *height) noexcept override;
    // bool guiSetSize(uint32_t width, uint32_t height) noexcept override;
    // void guiSuggestTitle(const char *title) noexcept override;
    bool guiSetParent(const clap_window *window) noexcept override
    {
        if (!guiProvider)
        {
            hostMisbehaving("Gui not created");
            return false;
        }
        return guiProvider->setParent(window);
    }
};
} // namespace free_audio::cvp
#endif // CVP_PLUGIN_BASE_H
