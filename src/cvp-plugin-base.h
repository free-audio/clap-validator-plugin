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

#include "cvp-plugin.h"
#include "clap/helpers/param-queue.hh"
#include "gui/gui.h"
#include <fmt/core.h>
#include "spsc-lock-free-queue.h"

namespace free_audio::cvp
{
static constexpr clap::helpers::MisbehaviourHandler misLevel =
    clap::helpers::MisbehaviourHandler::Ignore;
static constexpr clap::helpers::CheckingLevel checkLevel = clap::helpers::CheckingLevel::Maximal;

using plugHelper_t = clap::helpers::Plugin<misLevel, checkLevel>;

struct CVPClap : public plugHelper_t
{
    ValidatorFlavor flavor;
    CVPClap(const clap_host *h, ValidatorFlavor inFlavor)
        : flavor(inFlavor), plugHelper_t(getDescriptor(inFlavor), h)
    {
        logFmt(CLAP_LOG_INFO, "CVPClap created: Flavor={} Name={}", (int)flavor,
               getDescriptor(flavor)->name);
    }

    // This is not exactly realtime safe
    template <typename... Args>
    void logFmt(clap_log_severity s, const char *fmt, Args &&...args) const noexcept
    {
        auto res = fmt::format(fmt, std::forward<Args>(args)...);
        log(s, res.c_str());
    }

    std::vector<std::string> logLines;
    detail::spsc_lockfree_queue<clap_event_transport, 1024> transportQueue;

  protected:
    bool init() noexcept override
    {
        logFmt(CLAP_LOG_INFO, "init");

        auto probe = [this](auto ext)
        {
            if (_host.host()->get_extension(_host.host(), ext))
            {
                logFmt(CLAP_LOG_INFO, "Supports Extension: {}", ext);
            }
            else
            {
                logFmt(CLAP_LOG_INFO, "Missing Extension: {}", ext);
            }
        };
        probe(CLAP_EXT_PARAMS);
        probe(CLAP_EXT_GUI);
        probe(CLAP_EXT_STATE);
        return true;
    }
    bool activate(double sampleRate, uint32_t minFrameCount,
                  uint32_t maxFrameCount) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "{} sampleRate={} minFrameCount={} maxFrameCount={}", __func__,
               sampleRate, minFrameCount, maxFrameCount);
        return true;
    }
    void deactivate() noexcept override { logFmt(CLAP_LOG_INFO, "{}", __func__); }
    bool startProcessing() noexcept override
    {
        logFmt(CLAP_LOG_INFO, "{}", __func__);
        return true;
    }
    void stopProcessing() noexcept override { logFmt(CLAP_LOG_INFO, "{}", __func__); }

    std::mutex logLinesMutex;
    void logTee(clap_log_severity severity, const char *msg) const noexcept override
    {
        std::ostringstream oss;
        switch (severity)
        {
        case CLAP_LOG_INFO:
            oss << "[INFO   ] ";
            break;
        case CLAP_LOG_WARNING:
            oss << "[WARNING] ";
            break;
        case CLAP_LOG_ERROR:
            oss << "[ERROR  ] ";
            break;
        case CLAP_LOG_DEBUG:
            oss << "[DEBUG  ] ";
            break;
        default:
            oss << "[UNKNOWN] ";
            return;
        }
        oss << msg;
        std::cout << oss.str() << std::endl;

        // This is so gross. log functions are only const in the most
        // c++-thinks-sideeffects-are-free sense but that has sort of polluted the clap helpers api.
        // So
        auto *ncv = const_cast<CVPClap *>(this);
        std::lock_guard<std::mutex> lock(ncv->logLinesMutex);
        ncv->logLines.push_back(oss.str());
        if (guiProvider)
            guiProvider->logDirty = true;
    }
    bool implementsAudioPorts() const noexcept override
    {
        logFmt(CLAP_LOG_INFO, "{}", __func__);
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
        logFmt(CLAP_LOG_INFO, "{} isInput={}", __func__, isInput);
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
        logFmt(CLAP_LOG_INFO, "{} index={} isInput={}", __func__, index, isInput);
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
        logFmt(CLAP_LOG_INFO, "{}", __func__);
        if (flavor == ValidatorFlavor::StereoEffect)
            return false;
        return true;
    }
    uint32_t notePortsCount(bool isInput) const noexcept override
    {
        logFmt(CLAP_LOG_INFO, "{} isInput={}", __func__, isInput);

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
        logFmt(CLAP_LOG_INFO, "{} index={} isInput={}", __func__, index, isInput);

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
        guiProvider = gui::createGuiProvider(this);
        return guiProvider != nullptr;
    }
    void guiDestroy() noexcept override { guiProvider.reset(); }

    bool guiSetScale(double scale) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui SetScale {}", scale);
        return guiProvider && guiProvider->setScale(scale);
    }
    bool guiShow() noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui Show");
        return guiProvider && guiProvider->show();
    }

    bool guiHide() noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui Hide");
        return guiProvider && guiProvider->hide();
    }
    bool guiGetSize(uint32_t *width, uint32_t *height) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui GetSize");
        return guiProvider && guiProvider->getSize(width, height);
    }
    bool guiCanResize() const noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui CanResize");
        return guiProvider && guiProvider->canResize();
    }
    bool guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui GetResizeHints");
        return guiProvider && guiProvider->getResizeHints(hints);
    }
    bool guiAdjustSize(uint32_t *width, uint32_t *height) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui AdjustSize");
        return guiProvider && guiProvider->adjustSize(width, height);
    }
    bool guiSetSize(uint32_t width, uint32_t height) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui SetSize w={} h={}", width, height);
        return guiProvider && guiProvider->setSize(width, height);
    }
    void guiSuggestTitle(const char *title) noexcept override
    {
        logFmt(CLAP_LOG_INFO, "Gui SuggestTitle {}", title);
        if (!guiProvider)
            return;
        guiProvider->suggestTitle(title);
    }
    bool guiSetParent(const clap_window *window) noexcept override
    {
        if (!guiProvider)
        {
            hostMisbehaving("Gui not created");
            return false;
        }
        return guiProvider->setParent(window);
    }

    uint32_t processEvery{0};
    void processPrecursor(const clap_process_t *proc)
    {
        if ((processEvery == 0) && proc->transport && guiProvider)
        {
            transportQueue.enqueue(*(proc->transport));
        }
        processEvery = (processEvery + 1) & (16 - 1);
    }
};

namespace gui
{
template <typename... Args>
void GuiProvider::logFmt(clap_log_severity s, const char *fmt, Args &&...args) const noexcept
{
    if (parent)
        parent->logFmt(s, fmt, std::forward<Args>(args)...);
}
} // namespace gui
} // namespace free_audio::cvp
#endif // CVP_PLUGIN_BASE_H
