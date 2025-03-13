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

#ifndef FREEAUDIO_CVP_INSTANCES_NOTE_GENERATOR_H
#define FREEAUDIO_CVP_INSTANCES_NOTE_GENERATOR_H

#include "cvp-plugin-base.h"

namespace free_audio::cvp
{
struct NoteGeneratorPlugin : CVPClap<ValidatorFlavor::NoteGenerator>
{
    explicit NoteGeneratorPlugin(const clap_host_t *host)
        : CVPClap<ValidatorFlavor::NoteGenerator>(host)
    {
    }
};
} // namespace free_audio::cvp

#endif // STEREO_GENERATOR_H
