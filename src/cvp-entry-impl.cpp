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

#include "cvp-entry-impl.h"

namespace free_audio::cvp
{
bool clap_init(const char *p) { return true; }
void clap_deinit() {}

const void *get_factory(const char *factory_id) { return nullptr; }
} // namespace free_audio::cvp