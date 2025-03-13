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

#ifndef FREEAUDIO_CVP_INSTANCES_STEREO_EFFECT_H
#define FREEAUDIO_CVP_INSTANCES_STEREO_EFFECT_H

#include "cvp-plugin-base.h"

namespace free_audio::cvp
{
struct StereoEffectPlugin : CVPClap<ValidatorFlavor::StereoEffect>
{
    explicit StereoEffectPlugin(const clap_host_t *host)
        : CVPClap<ValidatorFlavor::StereoEffect>(host)
    {
    }
};
} // namespace free_audio::cvp

#endif // STEREO_Effect_H
