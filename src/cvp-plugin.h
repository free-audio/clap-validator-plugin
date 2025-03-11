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

#ifndef FREEAUDIO_CVP_CVP_PLUGIN_H
#define FREEAUDIO_CVP_CVP_PLUGIN_H

#include <clap/clap.h>

namespace free_audio::cvp
{
enum ValidatorFlavor
{
    StereoEffect = 0,    // audio in-out no note ports
    StereoGenerator = 1, // audio out, note in
    NoteGenerator = 2,   // no audio, note out
    NoteTransformer = 3, // no audio, note in-out
};

const clap_plugin_descriptor *getDescriptor(ValidatorFlavor);
const clap_plugin *makePlugin(const clap_host *, ValidatorFlavor);
} // namespace free_audio::cvp
#endif
