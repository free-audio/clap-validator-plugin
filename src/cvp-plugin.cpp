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

#include "cvp-plugin.h"

namespace free_audio::cvp
{

const clap_plugin_descriptor *getDescriptor(ValidatorFlavor) { return nullptr; }
const clap_plugin *makePlugin(const clap_host *, ValidatorFlavor) { return nullptr; }
} // namespace free_audio::cvp