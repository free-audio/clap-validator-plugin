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

#include <clap/clap.h>
#include <clap/helpers/plugin.hxx>
#include <clap/helpers/host-proxy.hxx>

#include "cvp-plugin.h"

#include "instances/stereo-generator.h"
#include "instances/stereo-effect.h"
#include "instances/note-generator.h"
#include "instances/note-transformer.h"

namespace free_audio::cvp
{
const clap_plugin_descriptor *getDescriptor(ValidatorFlavor fl)
{
    switch (fl)
    {
    case ValidatorFlavor::StereoEffect:
    {
        static const char *features[] = {CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, nullptr};

        static clap_plugin_descriptor desc = {CLAP_VERSION,
                                              "org.free-audio.cvp.stereo-effect",
                                              "Clap Validator (Stereo Effect)",
                                              "The Clap Team",
                                              "https://www.cleveraudio.org",
                                              "",
                                              "",
                                              "0.0.1",
                                              "Clap Validator Plugin - Effect Version",
                                              &features[0]};
        return &desc;
    }
    case ValidatorFlavor::StereoGenerator:
    {
        static const char *features[] = {CLAP_PLUGIN_FEATURE_INSTRUMENT, nullptr};

        static clap_plugin_descriptor desc = {CLAP_VERSION,
                                              "org.free-audio.cvp.stereo-generator",
                                              "Clap Validator (Stereo Generator)",
                                              "The Clap Team",
                                              "https://www.cleveraudio.org",
                                              "",
                                              "",
                                              "0.0.1",
                                              "Clap Validator Plugin - Generator Version",
                                              &features[0]};
        return &desc;
    }
    case ValidatorFlavor::NoteGenerator:
    {
        static const char *features[] = {CLAP_PLUGIN_FEATURE_NOTE_EFFECT, nullptr};

        static clap_plugin_descriptor desc = {CLAP_VERSION,
                                              "org.free-audio.cvp.note-generator",
                                              "Clap Validator (Note Generator)",
                                              "The Clap Team",
                                              "https://www.cleveraudio.org",
                                              "",
                                              "",
                                              "0.0.1",
                                              "Clap Validator Plugin - Note Generator Version",
                                              &features[0]};
        return &desc;
    }
    case ValidatorFlavor::NoteTransformer:
    {
        static const char *features[] = {CLAP_PLUGIN_FEATURE_NOTE_EFFECT, nullptr};

        static clap_plugin_descriptor desc = {CLAP_VERSION,
                                              "org.free-audio.cvp.note-transformer",
                                              "Clap Validator (Note Transformer)",
                                              "The Clap Team",
                                              "https://www.cleveraudio.org",
                                              "",
                                              "",
                                              "0.0.1",
                                              "Clap Validator Plugin - Note Transformer Version",
                                              &features[0]};
        return &desc;
    }
    }
    return nullptr;
}

const clap_plugin *makePlugin(const clap_host *h, ValidatorFlavor fl)
{
    switch (fl)
    {
    case ValidatorFlavor::StereoEffect:
    {
        auto res = new StereoEffectPlugin(h);
        return res->clapPlugin();
    }
    case ValidatorFlavor::StereoGenerator:
    {
        auto res = new StereoGeneratorPlugin(h);
        return res->clapPlugin();
    }
    case ValidatorFlavor::NoteGenerator:
    {
        auto res = new NoteGeneratorPlugin(h);
        return res->clapPlugin();
    }
    case ValidatorFlavor::NoteTransformer:
    {
        auto res = new NoteTransformerPlugin(h);
        return res->clapPlugin();
    }
    }
    return nullptr;
}
} // namespace free_audio::cvp