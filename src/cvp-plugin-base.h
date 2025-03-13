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
    void logTee(clap_log_severity severity, const char *msg) const noexcept override
    {
        std::cout << (int)severity << " " << msg << std::endl;
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
};
} // namespace free_audio::cvp
#endif // CVP_PLUGIN_BASE_H
